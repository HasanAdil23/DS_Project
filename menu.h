#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include "gameaudio.h" 
using namespace std;
using namespace sf;

class GameMenu {
private:
    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Font font;
    sf::Text menuItems[4];
    int selectedItemIndex;
    bool showAnimationDone = false;

    

public:
    GameAudio* audioPlayer;
    GameMenu(sf::RenderWindow& win, GameAudio* audio = nullptr);
    void loadResources();
    void setupMenu();
    void navigateMenu(sf::Keyboard::Key key);
    int handleInput();
    void drawMenu();
    void stopaudio()
    {
        audioPlayer->stopmenumusic();
    }
};

// Constructor
GameMenu::GameMenu(sf::RenderWindow& win, GameAudio* audio): window(win), selectedItemIndex(0), audioPlayer(audio)
{
    loadResources();
    setupMenu();
    audio->playMenumusic();
}

// Load background and font
void GameMenu::loadResources() {
    
    if (!backgroundTexture.loadFromFile("images/t1.jpg")) {
        std::cerr << "Failed to load background image\n";
    }
    else {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setColor(sf::Color(255, 255, 255, 100)); // Optional: transparency

        // 🔥 Scale background to fit the window
        Vector2u textureSize = backgroundTexture.getSize();
        Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            float(windowSize.x) / textureSize.x,
            float(windowSize.y) / textureSize.y
        );
    }

    if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
        std::cerr << "Failed to load font\n";
    }
}

// Setup menu items
void GameMenu::setupMenu() {
    std::string options[] = { "PLAY", "Highscores", "Leaderboard", "QUIT" };
    float windowWidth = 40 * 18;

    for (int i = 0; i < 4; ++i) {
        menuItems[i].setFont(font);
        menuItems[i].setString(options[i]);
        menuItems[i].setCharacterSize(30);

        sf::FloatRect textRect = menuItems[i].getLocalBounds();
        float x = (windowWidth - textRect.width) / 2.f;
        float y = 200.f + i * 60.f;

        menuItems[i].setPosition(x, y);
        menuItems[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
    }
}

// Navigate through menu items
void GameMenu::navigateMenu(sf::Keyboard::Key key) {
    menuItems[selectedItemIndex].setFillColor(sf::Color::White);

    if (key == sf::Keyboard::Up) {
        selectedItemIndex = (selectedItemIndex - 1 + 4) % 4;
    }
    else if (key == sf::Keyboard::Down) {
        selectedItemIndex = (selectedItemIndex + 1) % 4;
    }

    if (audioPlayer) audioPlayer->playNavigationSound(); 

    menuItems[selectedItemIndex].setFillColor(sf::Color::Yellow);
}

// Handle keyboard input
int GameMenu::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return -1;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
                navigateMenu(event.key.code);
            }
            else if (event.key.code == sf::Keyboard::Enter) {
                return selectedItemIndex;
            }
        }
    }
    return -2;
}

// Draw background, title, and menu
void GameMenu::drawMenu() {
    window.clear();
    window.draw(backgroundSprite);

    float windowWidth = 40 * 18;
    float yPos = 80.f;
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Red);
    titleText.setPosition(windowWidth / 2.f, yPos);

    if (!showAnimationDone) {
        std::string title = "XONIX";
        Clock letterClock;

        for (std::size_t i = 0; i < title.size(); ++i) {
            while (letterClock.getElapsedTime().asSeconds() < (i + 1) * 0.2f) {
                window.clear();
                window.draw(backgroundSprite);

                std::string visible = title.substr(0, i + 1);
                titleText.setString(visible);
                sf::FloatRect bounds = titleText.getLocalBounds();
                titleText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                titleText.setPosition(windowWidth / 2.f, yPos);
                window.draw(titleText);
                window.display();
            }
        }

        sf::sleep(sf::seconds(0.5f));
        showAnimationDone = true;
    }

    // Draw full title
    titleText.setString("XONIX");
    sf::FloatRect finalBounds = titleText.getLocalBounds();
    titleText.setOrigin(finalBounds.width / 2.f, finalBounds.height / 2.f);
    titleText.setPosition(windowWidth / 2.f, yPos);
    window.draw(titleText);

    // Draw menu options
    for (int i = 0; i < 4; ++i)
        window.draw(menuItems[i]);

    window.display();
}
