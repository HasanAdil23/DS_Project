#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "scoresystem.h"
#include "gameaudio.h"

using namespace std;

const int Ma = 25;
const int Na = 40;
const int TILE_SIZEa = 18;
const int MAX_ENEMIES = 5;

struct MultiplayerEntity {
    int x, y, dx, dy;
    int tileValue;
    bool active;
    bool constructing;
    ScoreSystem* score;
    std::string playerName;
    bool frozen;
    float freezeTimer;
};

struct MultiplayerEnemy {
    int x, y, dx, dy;
    bool active;
};

class MultiplayerGame {
private:
    MultiplayerEntity p1, p2;
    MultiplayerEnemy enemies[MAX_ENEMIES];
    int(&grid)[Ma][Na];
    GameAudio audio;

public:
    MultiplayerGame(int(&g)[Ma][Na], ScoreSystem* s1, ScoreSystem* s2,
        const std::string& name1, const std::string& name2)
        : grid(g) {
        srand(time(0));
        p1 = { 5, 5, 0, 0, 2, true, false, s1, name1, false, 0 };
        p2 = { Na - 6, Ma - 6, 0, 0, 3, true, false, s2, name2, false, 0 };

        // Initialize enemies
        for (int i = 0; i < MAX_ENEMIES; ++i) 
        {
            //enemies[i] = { rand() % Na, rand() % Ma, 1, 0, true };
            enemies[i].x = rand() % Na;
            enemies[i].y = rand() % Ma;
            enemies[i].dx = 1;
            enemies[i].dy = 0;
            enemies[i].active = true;
        }
    }

    void handleInput() {
        if (!p1.frozen) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { p1.dx = -1; p1.dy = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { p1.dx = 1;  p1.dy = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { p1.dx = 0;  p1.dy = -1; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { p1.dx = 0;  p1.dy = 1; }
        }
        if (!p2.frozen) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { p2.dx = -1; p2.dy = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { p2.dx = 1;  p2.dy = 0; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { p2.dx = 0;  p2.dy = -1; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { p2.dx = 0;  p2.dy = 1; }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !p1.frozen && p1.score->getPowerUpCount() > 0) {
            p1.frozen = true;
            p1.freezeTimer = 3.0f;
            p1.score->usePowerUp();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && !p2.frozen && p2.score->getPowerUpCount() > 0) 
        {
            p2.frozen = true;
            p2.freezeTimer = 3.0f;
            p2.score->usePowerUp();
        }
    }

    void update(float timer) 
    {
        if (p1.frozen) 
        {
            p1.freezeTimer -= timer;
            if (p1.freezeTimer <= 0) p1.frozen = false;
        }

        if (p2.frozen)
        {
            p2.freezeTimer -= timer;
            if (p2.freezeTimer <= 0)
                p2.frozen = false;
        }

        movePlayer(p1);
        movePlayer(p2);
        if (!p1.frozen && !p2.frozen)
            moveEnemies();

        checkCollisions();
    }

    void movePlayer(MultiplayerEntity& player) {
        if (!player.active) return;

        int nx = player.x + player.dx;
        int ny = player.y + player.dy;

        if (nx < 0 || nx >= Na || ny < 0 || ny >= Ma)
            return;

        player.x = nx;
        player.y = ny;

        if (grid[ny][nx] == 0) {
            grid[ny][nx] = player.tileValue;
            player.constructing = true;
            player.score->addPoints(1);
        }
    }

    void moveEnemies() {
        for (int i = 0; i < MAX_ENEMIES; ++i) {
            if (!enemies[i].active) continue;

            int dir = rand() % 4;
            int dx = (dir == 0) - (dir == 1);
            int dy = (dir == 2) - (dir == 3);

            int nx = enemies[i].x + dx;
            int ny = enemies[i].y + dy;

            if (nx >= 0 && nx < Na && ny >= 0 && ny < Ma) {
                enemies[i].x = nx;
                enemies[i].y = ny;
            }
        }
    }

    void checkCollisions() {
        for (int i = 0; i < MAX_ENEMIES; ++i) {
            if (!enemies[i].active) continue;

            if ((enemies[i].x == p1.x && enemies[i].y == p1.y && p1.constructing) ||
                (grid[enemies[i].y][enemies[i].x] == p1.tileValue)) {
                p1.active = false;
            }

            if ((enemies[i].x == p2.x && enemies[i].y == p2.y && p2.constructing) ||
                (grid[enemies[i].y][enemies[i].x] == p2.tileValue)) {
                p2.active = false;
            }
        }

        if (p1.x == p2.x && p1.y == p2.y && p1.constructing && p2.constructing) {
            p1.active = false;
            p2.active = false;
        }

        if (grid[p1.y][p1.x] == p2.tileValue && p1.constructing)
            p1.active = false;

        if (grid[p2.y][p2.x] == p1.tileValue && p2.constructing)
            p2.active = false;
    }

    bool isGameOver() const
    {
        return !(p1.active && p2.active);
    }

    std::string determineWinner() 
    {
        if (p1.score->getScore() > p2.score->getScore()) return p1.playerName;
        else if (p2.score->getScore() > p1.score->getScore()) return p2.playerName;
        else return "Draw";
    }

    void drawEntities(sf::RenderWindow& window, sf::Sprite& tileSprite) {
        tileSprite.setTextureRect(sf::IntRect(36, 0, TILE_SIZEa, TILE_SIZEa));

        if (p1.active) {
            tileSprite.setColor(sf::Color::Red);
            tileSprite.setPosition(p1.x * TILE_SIZEa, p1.y * TILE_SIZEa);
            window.draw(tileSprite);
        }

        if (p2.active) {
            tileSprite.setColor(sf::Color::Blue);
            tileSprite.setPosition(p2.x * TILE_SIZEa, p2.y * TILE_SIZEa);
            window.draw(tileSprite);
        }

        tileSprite.setColor(sf::Color::Green);
        for (int i = 0; i < MAX_ENEMIES; ++i) {
            if (!enemies[i].active) continue;
            tileSprite.setPosition(enemies[i].x * TILE_SIZEa, enemies[i].y * TILE_SIZEa);
            window.draw(tileSprite);
        }

        tileSprite.setColor(sf::Color::White);
    }

    void run(sf::RenderWindow& window, sf::Texture& tileTex, sf::Texture& enemyTex)
    {
        sf::Sprite sTile(tileTex);
        sf::Sprite sEnemy(enemyTex);
        sEnemy.setOrigin(20, 20);

        sf::Clock clock;
        float timer = 0, delay = 0.07f;

        audio.playBackgroundMusic(); // Optional background music

        while (window.isOpen()) {
            float time = clock.restart().asSeconds();
            timer += time;

            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed)
                    return;
            }

            handleInput();

            if (timer > delay) {
                update(timer);
                timer = 0;
            }

            // === DRAW PHASE ===
            window.clear();

            for (int i = 0; i < Ma; ++i) {
                for (int j = 0; j < Na; ++j) {
                    if (grid[i][j] == 0) continue;
                    if (grid[i][j] == 1) sTile.setTextureRect(sf::IntRect(0, 0, TILE_SIZEa, TILE_SIZEa));
                    if (grid[i][j] == 2) sTile.setTextureRect(sf::IntRect(18, 0, TILE_SIZEa, TILE_SIZEa));
                    if (grid[i][j] == 3) sTile.setTextureRect(sf::IntRect(54, 0, TILE_SIZEa, TILE_SIZEa));

                    sTile.setPosition(j * TILE_SIZEa, i * TILE_SIZEa);
                    window.draw(sTile);
                }
            }

            // Draw players
            sTile.setTextureRect(sf::IntRect(36, 0, TILE_SIZEa, TILE_SIZEa));
            if (p1.active) {
                sTile.setColor(sf::Color::Red);
                sTile.setPosition(p1.x * TILE_SIZEa, p1.y * TILE_SIZEa);
                window.draw(sTile);
            }
            if (p2.active) {
                sTile.setColor(sf::Color::Blue);
                sTile.setPosition(p2.x * TILE_SIZEa, p2.y * TILE_SIZEa);
                window.draw(sTile);
            }
            sTile.setColor(sf::Color::White); // reset color

            // Draw enemies
            sEnemy.rotate(10); // for animation
            for (int i = 0; i < MAX_ENEMIES; ++i) {
                if (!enemies[i].active) continue;
                sEnemy.setPosition(enemies[i].x * TILE_SIZEa, enemies[i].y * TILE_SIZEa);
                window.draw(sEnemy);
            }

            // Draw score
            p1.score->draw(window);
            p2.score->draw(window);

            window.display();

            // === END CONDITIONS ===
            if (isGameOver()) {
                sf::sleep(sf::seconds(1));
                std::string winner = determineWinner();

                sf::Font font;
                font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf");

                sf::Text endText;
                endText.setFont(font);
                endText.setCharacterSize(30);
                endText.setFillColor(sf::Color::Yellow);
                endText.setString("Winner: " + winner);
                endText.setPosition(180, 200);

                window.clear();
                window.draw(endText);
                window.display();
                sf::sleep(sf::seconds(4));
                audio.stopBackgroundMusic();
                break;
            }
        }
    }
};