#pragma once
#include <SFML/Audio.hpp>
#include <sstream>
#include <iostream>
using namespace std;
using namespace sf;

class GameAudio 
{
private:
    SoundBuffer navBuffer;
    SoundBuffer gameOverBuffer;
    Music backgroundMusic;
    Music menumusic;
    Music loginmusic;
    Sound navSound;
    Sound gameOverSound;
    SoundBuffer loginErrorBuffer;
    Sound loginErrorSound;
    SoundBuffer quitSoundBuffer;
    Sound quitSound;
    Music endmenumusic;
    

public:
    GameAudio();
    bool loadSounds();

    void playNavigationSound();
    void playGameOverSound();
    void playBackgroundMusic();
    void stopBackgroundMusic();
    void playMenumusic();
    void playLoginmusic();
    void stopmenumusic();
    void stoploginMusic();
    void playLoginErrorSoundWithPauseResume();
    void playQuitSoundAndExit(RenderWindow& window);
    void playQuitSound();
    void stopQuitSound();

};

GameAudio::GameAudio() 
{
    loadSounds();
}

bool GameAudio::loadSounds()
{
    if (!navBuffer.loadFromFile("Audios/Somerandom.wav")) 
    {
        std::cerr << "Failed to load nav.wav\n";
        return false;
    }

    if (!gameOverBuffer.loadFromFile("Audios/GameOverSound.wav")) 
    {
        std::cerr << "Failed to load gameover.wav\n";
        return false;
    }

    if (!backgroundMusic.openFromFile("Audios/VegaStage.wav")) 
    {
        std::cerr << "Failed to load theme.ogg\n";
        return false;
    }

    if (!menumusic.openFromFile("Audios/Street Fighter II Arcade Music - Opening Theme - CPS1.wav"))
    {
        cerr << "Failed to load the menu music." << endl;
        return false;
    }

    if (!loginmusic.openFromFile("Audios/Tron Legacy - Soundtrack OST - 03 The Son of Flynn - Daft Punk.wav"))
    {
        cerr << "Failed to load the login screen music" << endl;
        return false;
    }
    if (!loginErrorBuffer.loadFromFile("Audios/Windows XP Error Sound.wav")) {
        cerr << "Failed to load LoginError.wav\n";
        return false;
    }

    if (!quitSoundBuffer.loadFromFile("Audios/Microsoft Windows XP Shutdown - Sound Effect (HD).wav")) {
        cerr << "Failed to load QuitSound.wav\n";
        return false;
    }

    if (!endmenumusic.openFromFile("Audios/(Music) Shinobi - BGM 1 (Arcade).wav"))
    {
        cerr << "End menu music is not being loaded" << endl;
    }    

    loginErrorSound.setBuffer(loginErrorBuffer);
    quitSound.setBuffer(quitSoundBuffer);

    navSound.setBuffer(navBuffer);
    gameOverSound.setBuffer(gameOverBuffer);

    return true;
}

void GameAudio::playQuitSound()
{
    endmenumusic.setLoop(true);
    endmenumusic.setVolume(100);
    endmenumusic.play();
}

void GameAudio::stopQuitSound()
{
    endmenumusic.stop();
}

void GameAudio::playNavigationSound() 
{
    navSound.play();
}

void GameAudio::playGameOverSound() 
{
    gameOverSound.play();
}

void GameAudio::playBackgroundMusic() 
{
    backgroundMusic.setLoop(true);
    backgroundMusic.setVolume(100); 
    backgroundMusic.play();
}

void GameAudio::stopBackgroundMusic() 
{
    backgroundMusic.stop();
}

void GameAudio::playMenumusic()
{
    menumusic.play();
    menumusic.setLoop(true);
    menumusic.setVolume(75);
}
void GameAudio::stopmenumusic()
{
    menumusic.stop();
}

void GameAudio::stoploginMusic()
{
    loginmusic.stop();
}

void GameAudio::playLoginmusic() 
{
    loginmusic.play();
    loginmusic.setLoop(true);
    loginmusic.setVolume(75);
}

void GameAudio::playLoginErrorSoundWithPauseResume() {
    loginmusic.pause();
    loginErrorSound.play();

    // Wait while the error sound plays
    while (loginErrorSound.getStatus() == Sound::Playing) {
        sf::sleep(sf::milliseconds(100));
    }

    loginmusic.play(); // Resume login music
}

void GameAudio::playQuitSoundAndExit(RenderWindow& window) {
    // Stop all ongoing music
    backgroundMusic.stop();
    menumusic.stop();
    loginmusic.stop();

    quitSound.play();

    // Wait while the quit sound plays
    while (quitSound.getStatus() == Sound::Playing) {
        sf::sleep(sf::milliseconds(100));
    }

    window.close(); // Close the game window
}
