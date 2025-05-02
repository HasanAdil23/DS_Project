#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "gameaudio.h"
using namespace sf;
using namespace std;

class EndMenu {
private:
    RenderWindow& window;
    Font font;
    Text titleText;
    Text scoreText;
    Text creditsText;
    Text menuOption;
    Texture backgroundtexture;
    Sprite Backgroundsprite;
    int selectedIndex;

public:
    GameAudio* audiosystem = nullptr;

    EndMenu(RenderWindow& win, int finalScore, GameAudio* audio = nullptr);
    void draw();
    int handleInput();
    void playexitmusic() { if (audiosystem) audiosystem->playQuitSound(); }
    void stopexitmusic() { if (audiosystem) audiosystem->stopQuitSound(); }
       
};

// --- Constructor ---
EndMenu::EndMenu(RenderWindow& win, int finalScore, GameAudio* audio)
    : window(win), selectedIndex(0), audiosystem(audio)
{
    if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
        cerr << "Failed to load font\n";
    }
    if (!backgroundtexture.loadFromFile("images/t1-negate.jpg")) 
    {
        cerr << "Failed to load end menu background\n";
    }
    
    Backgroundsprite.setTexture(backgroundtexture);
    Backgroundsprite.setColor(Color(255, 255, 255, 120)); 
    Vector2u textureSize = backgroundtexture.getSize();

    //making it fit
    Vector2u windowSize = window.getSize();
    Backgroundsprite.setScale( float(windowSize.x) / textureSize.x, float(windowSize.y) / textureSize.y );

    if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
        cerr << "Failed to load font\n";
    }
    float windowWidth = 40 * 18;

    // Title
    titleText.setFont(font);
    titleText.setString("GAME OVER");
    titleText.setCharacterSize(60);
    titleText.setFillColor(Color::Red);
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    titleText.setPosition(windowWidth / 2, 60.f);

    // Score
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::White);
    stringstream ss;
    ss << "SCORE: " << finalScore;
    scoreText.setString(ss.str());
    FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
    scoreText.setPosition(windowWidth / 2, 140.f);

    

    // Menu Option
    menuOption.setFont(font);
    menuOption.setString("RETURN TO MAIN MENU");
    menuOption.setCharacterSize(28);
    menuOption.setFillColor(Color::Yellow);
    FloatRect optionBounds = menuOption.getLocalBounds();
    menuOption.setOrigin(optionBounds.width / 2, optionBounds.height / 2);
    menuOption.setPosition(windowWidth / 2, 240.f);

    // Credits
    creditsText.setFont(font);
    creditsText.setString("XONIX by Hasan and Zain");
    creditsText.setCharacterSize(18);
    creditsText.setFillColor(Color::Red);
    FloatRect creditsBounds = creditsText.getLocalBounds();
    creditsText.setOrigin(creditsBounds.width / 2, creditsBounds.height / 2);
    creditsText.setPosition(windowWidth / 2, 420.f);
}

// --- Draw ---
void EndMenu::draw() {
    window.clear(Color(30, 30, 30));
    window.draw(Backgroundsprite);
    window.draw(titleText);
    window.draw(scoreText);
    window.draw(menuOption);
    window.draw(creditsText);
    window.display();
}

// --- Handle Input ---
int EndMenu::handleInput()
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
            return -1;
        }
        if (event.type == Event::KeyPressed) {
            if (event.key.code == Keyboard::Enter) {
                return 0; // Return to main menu
            }
        }
    }
    return -2;
}
