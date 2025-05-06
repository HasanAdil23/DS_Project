#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include "minheap.h"
using namespace std;
using namespace sf;

struct ScoreEntry 
{
    int id;
    std::string name;
    int score;
};

class ScoreSystem 
{
private:
    int score;
    int playerID;
    sf::Font font;
    sf::Text scoreText;
    minheap m1; //composition
    int rewardCounter;
    int powerUps;
    string username;
    int lastPowerUpScore;

    
public:
    
    void addPoints(int points);
    int getScore()const ;

    void draw(sf::RenderWindow& window); 
    void saveToFile(); // No parameter now

    void loadFromFile(); // optional, e.g., to show highscores. also uses max heap
    void processCapturedTiles(int tileCount); // Handles all bonus logic
    int getPowerUpCount() const;
    void usePowerUp(); // When used by player

    
    //hasan this is currently illegal. i think we're suppossed to sort using min and max heap
    void sortScores() 
    {
        const int MAX_ENTRIES = 100;
        ScoreEntry entries[MAX_ENTRIES];
        int count = 0;

        std::ifstream inFile("leaderboard.txt");
        if (!inFile.is_open()) {
            std::cout << "leaderboard.txt not found.\n";
            return;
        }

        while (inFile >> entries[count].name >> entries[count].id >> entries[count].score && count < MAX_ENTRIES) {
            count++;
        }
        inFile.close();

        // Bubble sort descending
        for (int i = 0; i < count - 1; ++i) {
            for (int j = 0; j < count - i - 1; ++j) {
                if (entries[j].score > entries[j + 1].score) {
                    ScoreEntry temp = entries[j];
                    entries[j] = entries[j + 1];
                    entries[j + 1] = temp;
                }
            }
        }

        std::ofstream outFile("leaderboard.txt");
        if (!outFile.is_open()) {
            std::cout << "Failed to open leaderboard.txt for writing.\n";
            return;
        }

        for (int i = 0; i < count; ++i) {
            outFile << entries[i].name << " " << entries[i].id << " " << entries[i].score << std::endl;
        }

        outFile.close();
    }

    int getPlayerTotalScore(int id) 
    {
        std::ifstream file("players/player_" + std::to_string(id) + ".txt");
        int total = 0, val;

        if (!file.is_open()) {
            std::cout << "No data for player ID " << id << std::endl;
            return 0;
        }

        while (file >> val) {
            total += val;
        }

        file.close();
        return total;
    }  

    ScoreSystem(int id, const std::string& name) : playerID(id), username(name), score(0), rewardCounter(0), powerUps(0), lastPowerUpScore(50)
    {
        if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf"))
        {
            std::cerr << "Failed to load font for score\n";
        }

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        scoreText.setString("SCORE: 0");
    }

    void reset();
};

void ScoreSystem::loadFromFile()
{
    std::ifstream file("leaderboard.txt");
    std::string name;
    int id, savedScore;

    std::cout << "\n----- Leaderboard -----\n";
    if (file.is_open()) {
        while (file >> name >> id >> savedScore) {
            std::cout << name << " (ID: " << id << "): " << savedScore << std::endl;
        }
        file.close();
    }
    else {
        std::cout << "No scores found.\n";
    }
}


void ScoreSystem::saveToFile()
{
    // Save to leaderboard (username ID score)
    std::ofstream leaderboard("leaderboard.txt", std::ios::app);
    if (leaderboard.is_open()) {
        leaderboard << username << " " << playerID << " " << score << std::endl;
        leaderboard.close();
    }

    // Save all-time score for the player
    std::string playerFilePath = "players/player_" + std::to_string(playerID) + ".txt";

    int previousTotal = 0;
    std::ifstream inFile(playerFilePath);
    if (inFile.is_open()) {
        inFile >> previousTotal;
        inFile.close();
    }

    int newTotal = previousTotal + score;

    std::ofstream outFile(playerFilePath, std::ios::trunc);
    if (outFile.is_open()) {
        outFile << newTotal << std::endl;
        outFile.close();
    }

    sortScores(); // optional
}

void ScoreSystem::processCapturedTiles(int tileCount) {
    int bonus = 1;
    if (rewardCounter >= 5 && tileCount > 5) {
        bonus = 4;
    }
    else if (rewardCounter >= 3 && tileCount > 5) {
        bonus = 2;
        rewardCounter++;
    }
    else if (tileCount > 10) {
        bonus = 2;
        rewardCounter++;
    }

    addPoints(tileCount * bonus);

    // Check if score threshold reached for power-up
    while (score >= lastPowerUpScore + 30) {
        powerUps++;
        lastPowerUpScore += 30;
        cout << "Power-Up awarded! Total: " << powerUps << "\n";
    }
}

int ScoreSystem::getPowerUpCount() const {
    return powerUps;
}

void ScoreSystem::usePowerUp() {
    if (powerUps > 0) {
        powerUps--;
        std::cout << "Power-Up used! Remaining: " << powerUps << "\n";
        // trigger 3s enemy freeze externally
    }
}

void ScoreSystem::reset() 
{
    score = 0;
    rewardCounter = 0;
    powerUps = 0;
    lastPowerUpScore = 50;
    scoreText.setString("Score: 0");
}

//updates scores on screen and on file
void ScoreSystem::addPoints(int points)
{
    score += points;
    //cout << "Score: " << score << endl;
    stringstream ss;
    ss << "Score: " << score;
    scoreText.setString(ss.str());
}

int ScoreSystem::getScore() const
{
    return score;
}

void ScoreSystem::draw(sf::RenderWindow& window)
{
    window.draw(scoreText);
}
