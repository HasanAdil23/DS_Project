#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
using namespace std;
using namespace sf;

class GameAudio 
{
private:
    sf::SoundBuffer navBuffer;
    sf::SoundBuffer gameOverBuffer;
    sf::Music backgroundMusic;

    sf::Sound navSound;
    sf::Sound gameOverSound;

public:
    GameAudio();
    bool loadSounds();

    void playNavigationSound();
    void playGameOverSound();
    void playBackgroundMusic();
    void stopBackgroundMusic();
};

GameAudio::GameAudio() {
    loadSounds();
}

bool GameAudio::loadSounds()
{
    if (!navBuffer.loadFromFile("Audios/Somerandom.mp3")) 
    {
        std::cerr << "Failed to load nav.wav\n";
        return false;
    }

    if (!gameOverBuffer.loadFromFile("Audios/GameOverSound.mp3")) 
    {
        std::cerr << "Failed to load gameover.wav\n";
        return false;
    }

    if (!backgroundMusic.openFromFile("Audios/VegaStage.mp3")) 
    {
        std::cerr << "Failed to load theme.ogg\n";
        return false;
    }

    navSound.setBuffer(navBuffer);
    gameOverSound.setBuffer(gameOverBuffer);

    return true;
}

void GameAudio::playNavigationSound() {
    navSound.play();
}

void GameAudio::playGameOverSound() {
    gameOverSound.play();
}

void GameAudio::playBackgroundMusic() {
    backgroundMusic.setLoop(true);
    backgroundMusic.setVolume(100); // Adjust as needed
    backgroundMusic.play();
}

void GameAudio::stopBackgroundMusic() {
    backgroundMusic.stop();
}