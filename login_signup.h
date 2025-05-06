#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "playerInfo.h"
#include "playerInfo.cpp"
#include "player_profile.h"
#include "gameaudio.h"  //music chalana hai 
#include <ctime>
#include <cstdlib>

using namespace sf;
using namespace std;

bool handleAuthentication(RenderWindow& window, string &loginName, string &loginID) 
{
    enum Screens { LOGIN, SIGNUP };
    Screens currentScreen = LOGIN;
    string inputName = "";
    string inputPass = "";
    string message = "";
    string playerIDmsg = "";
    bool showSignupOption = false;
    int playerID = -1;
    bool signupSuccess = false;
    bool returnedToLogin = false;
    bool count = 0;

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
    Font font;
    if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
        std::cerr << "Failed to load font\n";
    }
    Text title1("XONIX Login", font, 30);          
    title1.setPosition(235, 30);

    Text title2("XONIX Signup", font, 30);
    title2.setPosition(235, 30);

    Text nameLabel("Name: ", font, 20);           
    nameLabel.setPosition(50, 100);

    Text nameInput("", font, 20);                 
    nameInput.setPosition(150, 100);

    Text passLabel("Password: ", font, 20);       
    passLabel.setPosition(50, 150);

    Text passInput("", font, 20);                 
    passInput.setPosition(200, 150);

    Text loginBtn("LOGIN", font, 24);             
    loginBtn.setPosition(310, 200);

    Text errorMsg("", font, 18);                  
    errorMsg.setFillColor(Color::Red); 
    errorMsg.setPosition(50, 240);

    Text signupLink("Don't have an account? SIGNUP", font, 18); 
    signupLink.setFillColor(Color::Yellow); 
    signupLink.setPosition(50, 270);

    Text signupBtn("SIGNUP", font, 24);           
    signupBtn.setFillColor(Color::Red); 
    signupBtn.setPosition(310, 200);

    Text confirmMsg("", font, 18);                
    confirmMsg.setPosition(50, 240);

    loginBtn.setFillColor(Color::Red);

    bool typingName = true;
    bool typingPass = false;

    GameAudio loginmusic;
    loginmusic.playLoginmusic();

    while (window.isOpen()) 
    {
        Event e;
        while (window.pollEvent(e)) 
        {
            if (e.type == Event::Closed)
            {
                window.close();
                return false;
            }

            if (e.type == Event::MouseButtonPressed) 
            {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (currentScreen == LOGIN)
                {
                    if (loginBtn.getGlobalBounds().contains(mousePos))
                    {
                        ifstream fin("players.txt");
                        string uname, pword;
                        int id;
                        bool found = false;
                        while (fin >> uname >> pword >> id) 
                        {
                            if (uname == inputName && pword == inputPass) 
                            {
                                playerID = id;

                                loginName = inputName;     //for player profile
                                loginID = to_string(id);   //for player profile

                                found = true;
                                break;
                            }
                        }
                        fin.close();

                        if (found)
                        {
                            loginmusic.stoploginMusic();
                            return true;
                        }
                           

                        else
                        {
                            errorMsg.setString("Invalid credentials!");
                            showSignupOption = true;
                            loginmusic.playLoginErrorSoundWithPauseResume();
                        }
                    }

                    else if (showSignupOption && signupLink.getGlobalBounds().contains(mousePos)) 
                    {
                        currentScreen = SIGNUP;
                        inputName = "";
                        inputPass = "";
                        typingName = true;
                        typingPass = false;
                        errorMsg.setString("");
                        showSignupOption = false;
                    }
                }

                else if (currentScreen == SIGNUP)
                {
                    if (signupBtn.getGlobalBounds().contains(mousePos))
                    {
                        if (!inputName.empty() && inputPass.length() >= 6)
                        {
                            ifstream check("players.txt");
                            string existingUser, existingPass;
                            int existingID;
                            bool userExists = false;
                            while (check >> existingUser >> existingPass >> existingID)
                            {
                                if (existingUser == inputName)
                                {
                                    userExists = true;
                                    break;
                                }
                            }
                            check.close();

                            if (userExists)
                            {
                                confirmMsg.setString("Username already exists.");
                            }
                            else
                            {
                                ofstream fout("players.txt", ios::app);
                                srand(time(0));
                                int newID = rand() % 9000 + 1000;
                                fout << inputName << " " << inputPass << " " << newID << endl;
                                fout.close();
                                playerIDmsg = "Registered! Player ID: " + to_string(newID) + " (Press Enter)";
                                confirmMsg.setString(playerIDmsg);
                                signupSuccess = true;
                                typingName = false;
                                typingPass = false;
                            }
                        }

                        else
                        {
                            confirmMsg.setString("Name and password must be valid (min 6 chars)");
                        }
                    }

                }


            }

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up)
                {
                    typingName = true;
                    typingPass = false;
                }

                if (e.key.code == Keyboard::Down)
                {
                    typingName = false;
                    typingPass = true;
                }

                if (currentScreen == SIGNUP && e.key.code == Keyboard::Enter)
                {
                    if (signupSuccess)                   //if signup completed, we go back to login page
                    {
                        currentScreen = LOGIN;
                        inputName = "";
                        inputPass = "";
                        typingName = true;
                        typingPass = false;
                        returnedToLogin = true;
                        signupSuccess = false;
                        confirmMsg.setString("");
                    }
                    else
                    {                                     //if signup not completed, we remain at signup page
                        if (typingName)
                        {
                            typingName = false;
                            typingPass = true;
                        }
                    }
                }

                if (currentScreen == LOGIN && e.key.code == Keyboard::Enter)
                {
                    if (typingName)
                    {
                        typingName = false;
                        typingPass = true;
                    }
                }

            }

            if (e.type == Event::TextEntered)
            {
                if (returnedToLogin)
                {
                    returnedToLogin = false;
                    continue;
                }


                if (e.text.unicode < 128)
                {
                    char ch = static_cast<char>(e.text.unicode);
                    if (ch == '\b')
                    {
                        if (typingName && !inputName.empty())
                            inputName.pop_back();

                        else if (typingPass && !inputPass.empty())
                            inputPass.pop_back();

                    }

                    else if (ch == '\r' || ch == '\n')
                    {
                        if (typingName)
                        {
                            typingName = false;
                            typingPass = true;
                        }
                     
                    }

                    else
                    {
                        if (typingName && inputName.size() < 20) 
                            inputName += ch;

                        else if (typingPass && inputPass.size() < 20) 
                            inputPass += ch;

                    }

                }
            
            }
        
        }

        

        nameInput.setString(inputName);
        passInput.setString(string(inputPass.length(), '*')); // Hide password

        window.clear(Color(200, 200, 255));

        window.clear();
        window.draw(backgroundSprite);

        if (currentScreen == LOGIN)
        {
            window.draw(title1);
            window.draw(nameLabel); window.draw(nameInput);
            window.draw(passLabel); window.draw(passInput);

            window.draw(loginBtn);
            window.draw(errorMsg);
            if (showSignupOption)
                window.draw(signupLink);

        }
        else if (currentScreen == SIGNUP) 
        {
            window.draw(title2);
            window.draw(nameLabel); window.draw(nameInput);
            window.draw(passLabel); window.draw(passInput);

            window.draw(signupBtn);
            window.draw(confirmMsg);
        }


        if (typingName)
        {
            nameInput.setFillColor(Color::Yellow);
            nameLabel.setFillColor(Color::Red);
        }
            
        else
        {
            nameInput.setFillColor(Color::White);
            nameLabel.setFillColor(Color::White);
        }
            
        if (typingPass)
        {
            passInput.setFillColor(Color::Yellow);
            passLabel.setFillColor(Color::Red);
        }
            
        else
        {
            passInput.setFillColor(Color::White);
            passLabel.setFillColor(Color::White);
        }
            
        window.display();
    }

    return false;
}