#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace sf;

class PlayerProfile
{
public:
    string name;
    string ID;
    int totalScore;
    string theme; 
    
    Font font;
    Text nameText, idText, scoreText, themeText, titleText;

    void loadPlayerScore()
    {
        totalScore = 0;

        // Build file path from ID
        string filePath = "players/player_" + ID + ".txt";

        ifstream file(filePath);
        if (!file.is_open()) {
            cout << "Couldn't open score file for player ID: " << ID << endl;
            return;
        }

        file >> totalScore; // Directly read the cumulative score
        file.close();
    }

    void setupText(Text& text, const string& str, float x, float y, int size = 24) 
    {
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(Color::White);
        text.setPosition(x, y);
    }

    PlayerProfile()
    {
        name = "";
        ID = "";
        theme = "";
    }

    PlayerProfile(const string& Pname, const string& Pid) 
    {
        name = Pname;
        ID = Pid;
        theme = "";

        if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf"))
        {
            cout << "Couldn't load font\n";
        }

        loadPlayerScore();

        setupText(titleText, "Player Profile", 235, 50, 32);
        setupText(nameText, "Name: " + name, 175, 120);
        setupText(idText, "Player ID: " + ID, 175, 170);
        setupText(scoreText, "Total Score: " + to_string(totalScore), 175, 220);
        setupText(themeText, "Theme: (Coming Soon)", 175, 270);
    }

    void setupProfile()
    {
        if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf"))
        {
            cout << "Couldn't load font\n";
        }

        loadPlayerScore();

        setupText(titleText, "Player Profile", 205, 50, 32);
        setupText(nameText, "Name: " + name, 175, 120);
        setupText(idText, "Player ID: " + ID, 175, 170);
        setupText(scoreText, "Total Score: " + to_string(totalScore), 175, 220);
        setupText(themeText, "Theme: (Coming Soon)", 175, 270);
    }

    void showProfile(RenderWindow& window)
    {
        while (window.isOpen())
        {

            Texture backgroundTexture;
            Sprite backgroundSprite;
            if (!backgroundTexture.loadFromFile("images/t1.jpg")) {
                cout << "Failed to load background image\n";
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

            Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                    return;
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(titleText);
            window.draw(nameText);
            window.draw(idText);
            window.draw(scoreText);
            window.draw(themeText);
            window.display();

        }
    }

};