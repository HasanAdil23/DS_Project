#ifndef MATCHMAKING_MULTIPLAYER_H
#define MATCHMAKING_MULTIPLAYER_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <cstring>
#include "scoresystem.h"
#include "login_signup.h"
#include "multiplayer.h"

using namespace std;

extern Texture t1, t3;


struct PlayerNode {
    char username[50];
    char password[50];
    int score;
    bool hasPlayed;
    PlayerNode* left;
    PlayerNode* right;

    PlayerNode(const char* u, const char* p, int s) {
        strcpy_s(username, u);
        strcpy_s(password, p);
        score = s;
        hasPlayed = false;
        left = nullptr;
        right = nullptr;
    }
};

class MatchmakingQueue {
    PlayerNode* root;

    PlayerNode* insert(PlayerNode* node, PlayerNode* newNode) {
        if (!node) 
            return newNode;

        if (newNode->score >= node->score)
            node->left = insert(node->left, newNode);
        else
            node->right = insert(node->right, newNode);
        return node;
    }

    void getTopTwo(PlayerNode* node, PlayerNode*& first, PlayerNode*& second) {
        if (!node) return;
        getTopTwo(node->left, first, second);
        if (!first && !node->hasPlayed) first = node;
        else if (!second && !node->hasPlayed && strcmp(first->username, node->username) != 0) second = node;
        getTopTwo(node->right, first, second);
    }

    void resetFlags(PlayerNode* node) {
        if (!node) return;
        resetFlags(node->left);
        resetFlags(node->right);
        node->hasPlayed = false;
    }

    void removeNode(PlayerNode*& root, const char* username) {
        if (!root) return;
        if (strcmp(username, root->username) < 0)
            removeNode(root->left, username);
        else if (strcmp(username, root->username) > 0)
            removeNode(root->right, username);
        else {
            if (!root->left) {
                PlayerNode* temp = root->right;
                delete root;
                root = temp;
            }
            else if (!root->right) {
                PlayerNode* temp = root->left;
                delete root;
                root = temp;
            }
            else {
                PlayerNode* temp = root->right;
                while (temp->left)
                    temp = temp->left;
                strcpy_s(root->username, temp->username);
                strcpy_s(root->password, temp->password);
                root->score = temp->score;
                removeNode(root->right, temp->username);
            }
        }
    }

    bool exists(PlayerNode* node, const char* uname) {
        if (!node)
            return false;

        if (strcmp(node->username, uname) == 0)
            return true;

        return exists(node->left, uname) || exists(node->right, uname);
    }

public:
    MatchmakingQueue() : root(nullptr) {}

    bool addPlayer(const char* uname, const char* ID) 
    {
        if (exists(root, uname))
            return false;

        std::string path = "players/player_";
        path += ID;
        path += ".txt";
        std::ifstream in(path);
        if (!in.is_open()) return false;

        int totalScore = 0;
        in >> totalScore;
        in.close();

        PlayerNode* newNode = new PlayerNode(uname, ID, totalScore);
        root = insert(root, newNode);
        return true;
    }

    bool isGamePossible() {
        PlayerNode* p1 = nullptr;
        PlayerNode* p2 = nullptr;
        getTopTwo(root, p1, p2);
        return (p1 && p2);
    }

    void playNextRound(sf::RenderWindow& window, ScoreSystem* score1, ScoreSystem* score2, int (& grid)[25][40])
    {
        PlayerNode* p1 = nullptr;
        PlayerNode* p2 = nullptr;
        getTopTwo(root, p1, p2);

        if (!p1 || !p2) {
            resetFlags(root);
            getTopTwo(root, p1, p2);
        }

        if (!p1 || !p2) return;

        p1->hasPlayed = true;
        p2->hasPlayed = true;

        MultiplayerGame game(grid, score1, score2, p1->username, p2->username);
        game.run(window, t1, t3);

        if (score1->getScore() > score2->getScore()) {
            removeNode(root, p2->username);
        }
        else {
            removeNode(root, p1->username);
        }
    }

    bool hasWinner(char* winnerName) {
        if (!root) return false;
        if (!root->left && !root->right) {
            strcpy_s(winnerName, 50, root->username);
            return true;
        }
        return false;
    }
};

MatchmakingQueue queue;

void runMultiplayerGame(sf::RenderWindow& mainWindow, int(&grid)[25][40])
{
    // Game logic
    sf::RenderWindow& window = mainWindow;

    sf::Font font;
    font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf");


    ScoreSystem s1(1, "Player1");
    ScoreSystem s2(2, "Player2");
    while (queue.isGamePossible()) {
        queue.playNextRound(mainWindow, &s1, &s2, grid);
    }

    char winner[50];
    if (queue.hasWinner(winner)) {
        sf::Text winText;
        winText.setFont(font);
        winText.setCharacterSize(30);
        winText.setFillColor(sf::Color::Yellow);
        winText.setPosition(200, 250);
        winText.setString(std::string("Winner: ") + winner);

        while (mainWindow.isOpen()) {
            sf::Event e;
            while (mainWindow.pollEvent(e)) {
                if (e.type == sf::Event::Closed) return;
                if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter)
                    return;
            }
            mainWindow.clear();
            mainWindow.draw(winText);
            mainWindow.display();
        }
    }
}

void multiplayerScreen(sf::RenderWindow& mainWindow, int(&grid)[25][40], const string& currentUsername, const string& currentID)
{
    sf::RenderWindow& window = mainWindow;
    //MatchmakingQueue queue;
    queue.addPlayer(currentUsername.c_str(), currentID.c_str()); // Add current user

    Texture backgroundTexture;
    Sprite backgroundSprite;
    if (!backgroundTexture.loadFromFile("images/t1.jpg")) {
        std::cerr << "Failed to load background image\n";
    }
    else {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setColor(sf::Color(255, 255, 255, 100)); // Optional: transparency

        //making it fit
        Vector2u textureSize = backgroundTexture.getSize();
        Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            float(windowSize.x) / textureSize.x,
            float(windowSize.y) / textureSize.y
        );
    }

    sf::Font font;
    font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf");

    sf::Text message1;
    message1.setFont(font);
    message1.setCharacterSize(27);
    message1.setFillColor(sf::Color::White);
    message1.setPosition(25, 20);
    message1.setString("Multiplayer Matchmaking");

    sf::Text message;
    message.setFont(font);
    message.setCharacterSize(18);
    message.setFillColor(sf::Color::White);
    message.setPosition(25, 80);
    message.setString("Enter username and password of other players.\nPress Right Shift to add the player.\nPress Enter to continue to game.");

    sf::String unameInput, passInput;
    sf::Text unameText, passText;
    unameText.setFont(font);
    passText.setFont(font);
    unameText.setCharacterSize(25);
    passText.setCharacterSize(25);
    unameText.setPosition(100, 260);
    passText.setPosition(130, 300);
    unameText.setFillColor(sf::Color::Green);
    passText.setFillColor(sf::Color::Green);

    bool enteringUsername = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return;

            else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode >= 32 && event.text.unicode <= 126) {
                    // Only allow visible ASCII chars
                    char enteredChar = static_cast<char>(event.text.unicode);
                    if (enteringUsername)
                        unameInput += enteredChar;
                    else
                        passInput += enteredChar;
                }
                else if (event.text.unicode == 8) { // Backspace
                    if (enteringUsername && !unameInput.isEmpty())
                        unameInput.erase(unameInput.getSize() - 1, 1);
                    else if (!enteringUsername && !passInput.isEmpty())
                        passInput.erase(passInput.getSize() - 1, 1);
                }
            }

            else if (event.type == sf::Event::KeyPressed)
            {

                auto code = event.key.code;

                if (event.key.code == sf::Keyboard::RShift) {
                    std::string uname = unameInput;
                    std::string pword = passInput;
                    /*if (!uname.empty() && !pword.empty()) {
                        queue.addPlayer(uname.c_str(), pword.c_str());
                    }

                    unameInput.clear();
                    passInput.clear();
                    enteringUsername = true;*/

                    if (!uname.empty() && !pword.empty())
                    {
                        bool ok = queue.addPlayer(uname.c_str(), pword.c_str());
                        if (ok) {
                            message.setString("Player added: " + uname);
                            // clear inputs so you can type the next one:
                            unameInput.clear();
                            passInput.clear();
                            enteringUsername = true;
                        }
                        else {
                            message.setString("Failed to add " + uname +
                                "\n(check file or duplicate?)");
                        }
                    }

                    

                }
                else if (event.key.code == sf::Keyboard::Enter) 
                {
                    runMultiplayerGame(window, grid);
                    return; // Exit input loop
                }
                else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
                    enteringUsername = !enteringUsername;
                }
            }
        }

        unameText.setString("Username: " + unameInput);
        passText.setString("ID: " + passInput);

        window.clear();
        window.draw(backgroundSprite);
        window.draw(message1);
        window.draw(message);
        window.draw(unameText);
        window.draw(passText);
        window.display();
    }

    // Game logic
    ScoreSystem s1(1, "Player1");
    ScoreSystem s2(2, "Player2");
    while (queue.isGamePossible()) 
    {
        queue.playNextRound(mainWindow, &s1, &s2, grid);
    }

    char winner[50];
    if (queue.hasWinner(winner)) 
    {
        sf::Text winText;
        winText.setFont(font);
        winText.setCharacterSize(30);
        winText.setFillColor(sf::Color::Yellow);
        winText.setPosition(200, 250);
        winText.setString(std::string("Winner: ") + winner);

        while (mainWindow.isOpen())
        {
            sf::Event e;
            while (mainWindow.pollEvent(e))
            {
                if (e.type == sf::Event::Closed)
                    return;
                if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter)
                    return;
            }
            mainWindow.clear();
            mainWindow.draw(winText);
            mainWindow.display();
        }
    }
}




#endif // MATCHMAKING_MULTIPLAYER_H