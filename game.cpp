#include <SFML/Graphics.hpp>
#include "login_signup.h"
#include "menu.h"
#include "player_profile.h"
#include "endmenu.h"
#include "gameaudio.h"
#include "gamestate.h"
#include "friend_system.h"
#include "inventory.h"
#include <string>
#include "leaderboardMgmt.h"
#include "scoresystem.h"
#include "multiplayer.h"
#include "matchmaking_queue.h"
#include <iostream>
#include <time.h>

using namespace sf;
using namespace std;

const int M = 25;
const int N = 40;

PlayerProfile loginPlayer;
PlayerManager pm;

Texture t1, t2, t3;

int grid[M][N] = { 0 };
int ts = 18; //tile size

//powerup 
bool enemiesFrozen = false;
float freezeTimer = 0.0f;
int Gtheme = 1;

void settheme(Texture& t, int id)
{
    //setting enemy sprites
    if (id == 1)
    {
        t.loadFromFile("images/enemy.png");
    }
    else if (id == 2)
    {
        t.loadFromFile("images/xonix_e2.png");
    }
    else if (id == 3)
    {
        t.loadFromFile("images/xonix_e1.png");
    }
    else if (id == 4)
    {
        t.loadFromFile("images/xonix_e3.png");
    }
}

struct Enemy
{
    int x, y, dx, dy;
    //bool active;

    Enemy()
    {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;
    }

    void move()
    {
        x += dx; if (grid[y / ts][x / ts] == 1) { dx = -dx; x += dx; }
        y += dy; if (grid[y / ts][x / ts] == 1) { dy = -dy; y += dy; }
    }
};

void drop(int y, int x)
{
    if (grid[y][x] == 0) grid[y][x] = -1;
    if (grid[y - 1][x] == 0) drop(y - 1, x);
    if (grid[y + 1][x] == 0) drop(y + 1, x);
    if (grid[y][x - 1] == 0) drop(y, x - 1);
    if (grid[y][x + 1] == 0) drop(y, x + 1);
}

void gridreset()
{
    // Reseting the grid to initial state
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;

}

//^^ --- These functions were given in the skeleton code ---
int main() 
{
    cout << "working" << endl;
    srand(time(0));

    RenderWindow window(VideoMode(N * ts, M * ts), "XONIX");
    window.setFramerateLimit(60);
    LeaderboardManager ldboard(window);
   
    // --- Proceeding to Login/Signup before starting the game ---

    if (!handleAuthentication(window, loginPlayer.name, loginPlayer.ID, loginPlayer.password))
        return 0;

    loadAllPlayers(pm);
    pm.addPlayer(loginPlayer.ID, loginPlayer.name);


    pm.loadFriendData("friends.txt");
    pm.loadPendingRequests("pending.txt");

    // Load game assets
    //using the global theme variable to set the enemy 
    //t3 is the enemy 

    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/gameover.png");
    settheme(t3, 1); //passing the default theme for now, in case the user does not go to the inventory 


    //this is a goto label, that i have created. used when user returns from the exitscreen
exit:
    // Reset grid to initial state (added because of the end menu)
    gridreset();
    GameAudio audio;
    // Show menu before starting the game
    GameMenu menu(window, &audio);
    int menuChoice = -2;
    
    //option 0 is for play. and so on
    while (window.isOpen() && menuChoice != 0) 
    {
        menuChoice = menu.handleInput();
        menu.drawMenu();

        if (menuChoice == 1)                  // For multiplayer mode
        {   
            multiplayerScreen(window, grid, loginPlayer.name, loginPlayer.ID);
            runMultiplayerGame(window, grid);
            menuChoice = -2;
        }

        else if (menuChoice == 2)
        {
            loginPlayer.themeid = Gtheme;
            loginPlayer.setupProfile();
            loginPlayer.showProfile(window);
        }

        else if (menuChoice == 3)
        {
            ldboard.displayLeaderboard(window);
            menuChoice = -2;
        }

        else if (menuChoice == 4) 
        {
            Font font;
            font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf");  //
            Player* me = pm.findPlayer(loginPlayer.ID);
            if (me) 
            {
                showFriendMenu(window, pm, me, font);
            }
            else
            {
                cout << "Player not found!\n";
            }
            menuChoice = -2;
        } 
        
		else if (menuChoice == 5)
        {
            // Load the AVL tree with default themes
            AVLTree tree;
            loadDefaultThemes(tree);
            // Show inventory window
            int selectedThemeID = showInventoryWindow(window, tree);
            if (selectedThemeID != -1) 
            {
                cout << "Selected Theme ID: " << selectedThemeID << endl;
                Gtheme = selectedThemeID;//setting the global theme variable
                cout<<"Theme ID set to: " << Gtheme << endl;
            }
            settheme(t3, Gtheme); // Loading the enemy texture based on the selected theme
        }
        else if (menuChoice == 6)                    //Quit option :/
        {
            pm.saveFriendData("friends.txt");
            pm.savePendingRequests("pending.txt");
            cout << "wo bhi apne na huay, dil bhi gaya haathon se" << endl;
            menu.audioPlayer->playQuitSoundAndExit(window); //windows xp wali awaaz yahan se chalti hai 
            window.close();
        }
    }

    if (menuChoice != 0 || !window.isOpen())
    {
        
        return 0;
    }

    
    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);

    int enemyCount = 6;
    Enemy a[10];

    bool Game = true;
    int x = 0, y = 0, dx = 0, dy = 0;
    float timer = 0, delay = 0.07;
    Clock clock;

    // Initialize grid borders
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    // Initialize score system
    ScoreSystem scoreSystem(stoi(loginPlayer.ID), loginPlayer.name); 

    menu.stopaudio();   //stopping the menu audio
    GameState currentState(stoi(loginPlayer.ID));

    /////////////SAVE GAME\\\\\\\\\\\ 
    if (currentState.hasSavedGame(stoi(loginPlayer.ID)))
    {
        if (currentState.promptResumeSFML(window))
        {
            // Resuming by placing tiles from saved state
            tilenode* curr = currentState.getHead();
            while (curr)
            {
                if (curr->grid)
                    grid[curr->y][curr->x] = 1;  
                curr = curr->next;
            }
            cout << "Resumed previous game state.\n";
            cout << "Printing linked list for debugging" << endl;
            currentState.printList();
            scoreSystem.reset();
            scoreSystem.setScore(currentState.getSavedScore());
        }
        else cout << "starting new game" << endl;

    }
	else
	{
        currentState.setSavedScore(0);
        scoreSystem.reset();
        scoreSystem.setScore(0);
	}
    
    audio.playBackgroundMusic();    //playing the game music
    // Main Game Loop
    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        //powerup 
        if (enemiesFrozen)
        {
            freezeTimer -= time;
            //counts down frame by frame, as per 60fps
            if (freezeTimer <= 0.0f) {
                enemiesFrozen = false;
            }
        }

        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
            {
                for (int i = 1; i < M - 1; i++)
                    for (int j = 1; j < N - 1; j++)
                        grid[i][j] = 0;

                x = 10; y = 0;
                Game = true;
                scoreSystem.reset();

                audio.stopBackgroundMusic();       // To stop already ringing music
                audio.playBackgroundMusic();       // Start music again
            }

            //power up handling. Key: P nika howay ya wada howay
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::P) {
                if (!enemiesFrozen && scoreSystem.getPowerUpCount() > 0) {
                    scoreSystem.usePowerUp();
                    enemiesFrozen = true;
                    freezeTimer = 3.0; // 3 seconds
                }
            }

            //save game 
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::S)
            {
                Font font;
                currentState.setSavedScore(scoreSystem.getScore());
                currentState.saveStateToFile();
                if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf"))
                {
                    cerr << "Failed to load font for resume prompt\n";
                }
                Text saveText;
                saveText.setFont(font);
                saveText.setString("Save ID: " + to_string(currentState.getsaveID()));
                saveText.setCharacterSize(24);
                saveText.setFillColor(Color::White);
                saveText.setPosition(100, 100);
                window.draw(saveText);
                window.display();
                sf::sleep(sf::seconds(10.0f)); // 10 second pause before clearing the message
                window.clear();
            }

        }

        if (Keyboard::isKeyPressed(Keyboard::Left)) { dx = -1; dy = 0; }
        if (Keyboard::isKeyPressed(Keyboard::Right)) { dx = 1; dy = 0; }
        if (Keyboard::isKeyPressed(Keyboard::Up)) { dx = 0; dy = -1; }
        if (Keyboard::isKeyPressed(Keyboard::Down)) { dx = 0; dy = 1; }

        if (!Game) continue;

        if (timer > delay)
        {
            x += dx;
            y += dy;

            if (x < 0) x = 0; if (x > N - 1) x = N - 1;
            if (y < 0) y = 0; if (y > M - 1) y = M - 1;

            if (grid[y][x] == 2) Game = false;

            if (grid[y][x] == 0)
            {
                grid[y][x] = 2;
                currentState.addTile(x, y, 2); // Adding the tile to linked list   )->for scoring
                scoreSystem.addPoints(1);  // Adding points for claiming tile      )->for scoring
            }

            timer = 0;
        }

        //enemies can only move if they are not frozen. powerup logic
        if (!enemiesFrozen)
        {
            for (int i = 0; i < enemyCount; i++)
            {
                a[i].move();
            }
        }

        if (grid[y][x] == 1)
        {
            dx = dy = 0;

            // Clear previous -1 in case of bug
            for (int i = 0; i < M; ++i)
                for (int j = 0; j < N; ++j)
                    if (grid[i][j] == -1)
                        grid[i][j] = 0;

            // Mark accessible area from each enemy
            for (int i = 0; i < enemyCount; i++)
            {
                int ey = a[i].y / ts;
                int ex = a[i].x / ts;
                if (grid[ey][ex] == 0)
                    drop(ey, ex);
            }

            int captured = 0;
            for (int i = 0; i < M; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    if (grid[i][j] == -1)
                    {
                        grid[i][j] = 0;
                    }
                    else if (grid[i][j] == 2)
                    {
                        grid[i][j] = 1;
                        captured++;
                    }
                    else if (grid[i][j] == 0)
                    {
                        grid[i][j] = 1;
                        captured++;
                    }
                }
            }
            // For points with bonus logic
            scoreSystem.processCapturedTiles(captured);
        }

        for (int i = 0; i < enemyCount; i++)
            if (grid[a[i].y / ts][a[i].x / ts] == 2)
                Game = false;

        // Draw game
        window.clear();

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0) continue;
                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
                sTile.setPosition(j * ts, i * ts);
                window.draw(sTile);
            }

        sTile.setTextureRect(IntRect(36, 0, ts, ts));
        sTile.setPosition(x * ts, y * ts);
        window.draw(sTile);

        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++)
        {
            sEnemy.setPosition(a[i].x, a[i].y);
            window.draw(sEnemy);
        }

        // Show score
        scoreSystem.draw(window);
        int scorefordisp = scoreSystem.getScore();
        if (!Game)
        {
            window.draw(sGameover);
            audio.stopBackgroundMusic();
            audio.playGameOverSound();
            scoreSystem.saveToFile();
            window.display(); // Make sure the GAME OVER screen is seen

            sf::sleep(sf::seconds(2.5f)); // Short pause before EndMenu

            // Show End Menu
            EndMenu endMenu(window, scorefordisp, &audio);
            int choice = -2;
            while (choice == -2 && window.isOpen())
            {
                choice = endMenu.handleInput();
                endMenu.draw();
            }
            // choice will always be 0 to Return to main
            goto exit; // to restart at the main menu


            break; // In case the game loop is still running
        }


        window.display();

    }


    return 0;
}
