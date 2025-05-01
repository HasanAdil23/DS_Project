#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include "minheap.h"
using namespace std;
using namespace sf;

class ScoreSystem 
{
private:
    int score;
    sf::Font font;
    sf::Text scoreText;
    minheap m1; //composition

public:
    ScoreSystem();

    void addPoints(int points);
    int getScore() const;

    void draw(sf::RenderWindow& window);
    void saveToFile(const std::string& playerName = "Player");
    void loadFromFile(); // optional, e.g., to show highscores. also uses max heap
  
    //hasan this is currently illegal. i think we're suppossed to sort using min and max heap
    void sortScores() 
    {
        const int MAX_ENTRIES = 100;
        int scores[MAX_ENTRIES];
        int count = 0;

        std::ifstream inFile("scores.txt");
        if (!inFile.is_open()) {
            std::cout << "scores.txt not found.\n";
            return;
        }

        // Read scores into array
        while (inFile >> scores[count] && count < MAX_ENTRIES) {
            count++;
        }
        inFile.close();

        // Bubble Sort (descending)
        for (int i = 0; i < count - 1; ++i) 
        {
            for (int j = 0; j < count - i - 1; ++j) 
            {
                if (scores[j] < scores[j + 1]) 
                {
                    int temp = scores[j];
                    scores[j] = scores[j + 1];
                    scores[j + 1] = temp;
                }
            }
        }

        // Write sorted scores back to file
        std::ofstream outFile("scores.txt");
        if (!outFile.is_open()) 
        {
            std::cout << "Failed to open scores.txt for writing.\n";
            return;
        }

        for (int i = 0; i < count; ++i) {
            outFile << scores[i] << std::endl;
        }

        outFile.close();
        //std::cout << "Scores sorted in descending order.\n";
    }

    //a function that reads scores from file and inserts them into min heap 
    void readScoresFromFile() 
    {
		std::ifstream inFile("scores.txt");
		if (!inFile.is_open()) 
        {
			std::cout << "Failed to open scores.txt\n";
			return;
		}

		int score;
		while (inFile >> score)
        {
			m1.insert(score);
		}
		inFile.close();
	}
    

    void reset();
};


ScoreSystem::ScoreSystem() : score(0) {
    if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
        std::cerr << "Failed to load font for score\n";
    }

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("SCORE: 0");
}

//updates scores on screen and on file
void ScoreSystem::addPoints(int points) 
{
    score += points;

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

void ScoreSystem::saveToFile(const std::string& playerName) 
{
    std::ofstream file("scores.txt", std::ios::app);
    if (file.is_open()) 
    {
        file  << score << std::endl;
        file.close();
    }
    sortScores(); 
}

void ScoreSystem::loadFromFile() 
{
    std::ifstream file("scores.txt");
    std::string name;
    int savedScore;

    std::cout << "\n----- Highscores -----\n";
    if (file.is_open()) {
        while (file >> name >> savedScore) 
        {
            std::cout << name << ": " << savedScore << std::endl;
        }
        file.close();
    }
    else 
    {
        std::cout << "No scores found.\n";
    }
}

void ScoreSystem::reset() 
{
    score = 0;
    scoreText.setString("Score: 0");
}
