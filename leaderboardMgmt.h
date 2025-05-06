#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
using namespace std;
using namespace sf;


// Stores player ID and score
struct score {
	int id;
	int pscore;
	score() {
		id = -1;
		pscore = -1;
	}
};

class minheap
{
	static const int maxsize = 10; // Leaderboard size
	int size = 0;
	score* heap;

	void swap(score& a, score& b)
	{
		score temp = a;
		a = b;
		b = temp;
	}

	void minheapify_up(int index) {
		while (index != 0) {
			int parent = (index - 1) / 2;
			if (heap[parent].pscore > heap[index].pscore) {
				swap(heap[parent], heap[index]);
				index = parent;
			}
			else break;
		}
	}

	void minheapify_down(int index) {
		int smallest = index;
		int left = 2 * index + 1;
		int right = 2 * index + 2;

		if (left < size && heap[left].pscore < heap[smallest].pscore)
			smallest = left;
		if (right < size && heap[right].pscore < heap[smallest].pscore)
			smallest = right;

		if (smallest != index) {
			swap(heap[smallest], heap[index]);
			minheapify_down(smallest);
		}
	}

	int findPlayerIndex(int id) {
		for (int i = 0; i < size; ++i)
			if (heap[i].id == id)
				return i;
		return -1;
	}

public:
	minheap() {
		heap = new score[maxsize];
	}

	void readFromFile()
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
	
	// Inserts or updates a score for a given player ID
	void insertOrUpdate(int playerID, int newScore) {
		int existingIndex = findPlayerIndex(playerID);

		if (existingIndex != -1) {
			// Player already exists — update score and reheap
			heap[existingIndex].pscore = newScore;
			minheapify_down(existingIndex);
			minheapify_up(existingIndex);
			return;
		}

		if (size < maxsize) {
			// Heap has space
			heap[size].id = playerID;
			heap[size].pscore = newScore;
			minheapify_up(size);
			size++;
		}
		else if (newScore > heap[0].pscore) {
			// Replace root (lowest score) with new high-scoring player
			heap[0].id = playerID;
			heap[0].pscore = newScore;
			minheapify_down(0);
		}
	}

	//print minheap
	void print()
	{
		for (int i = 0; i < size; i++)
			cout << heap[i].id << " " << heap[i].pscore << endl;
	}

	// Display leaderboard in descending order
	void display() {
		if (size == 0) {
			cout << "Leaderboard is empty.\n";
			return;
		}

		// Copy heap to temporary array
		score temp[maxsize];
		for (int i = 0; i < size; i++)
			temp[i] = heap[i];

		cout << "\n===== Top 10 Leaderboard =====\n";
		for (int i = 0; i < size; i++) {
			cout << i + 1 << ". Player ID: " << temp[i].id << " | Score: " << temp[i].pscore << endl;
		}
		cout << "==============================\n";
	}

	// Save leaderboard to a file
	void writeToFile() {
		ofstream outFile("leaderboard.txt");
		if (!outFile.is_open()) {
			cout << "Failed to open leaderboard.txt for writing.\n";
			return;
		}

		for (int i = 0; i < size; i++) {
			outFile << heap[i].id << " " << heap[i].pscore << endl;
		}
		outFile.close();
		cout << "Leaderboard written to file.\n";
	}

	~minheap() {
		delete[] heap;
	}
	// Returns leaderboard sorted in descending order (highest score first)
	int getSortedDescending(score* output) {
		// Copy current heap entries
		score temp[maxsize];
		for (int i = 0; i < size; ++i)
			temp[i] = heap[i];

		// Simple selection sort in descending order (since max 10 entries)
		for (int i = 0; i < size - 1; ++i) 
		{
			int maxIdx = i;
			for (int j = i + 1; j < size; ++j) 
			{
				if (temp[j].pscore > temp[maxIdx].pscore)
					maxIdx = j;
			}
			if (maxIdx != i)
				swap(temp[i], temp[maxIdx]);
		}

		// Copy sorted results to output
		for (int i = 0; i < size; ++i)
			output[i] = temp[i];

		return size; // Return number of valid entries
	}

};

class LeaderboardManager {
private:
    minheap leaderboard;  // Manages top 10 scores using min-heap
    sf::Font font;
    sf::Text titleText;
    sf::Text entryTexts[10];  // For displaying top 10 entries
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    float windowWidth = 40 * 18;
    float yPos = 80.f;
public:
    LeaderboardManager() 
    {

        if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
            std::cerr << "Failed to load font in LeaderboardManager\n";
        }

        titleText.setFont(font);
        titleText.setCharacterSize(28);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setString("LEADERBOARD");
        titleText.setPosition(215, 20);

        for (int i = 0; i < 10; ++i) 
        {
            entryTexts[i].setFont(font);
            entryTexts[i].setCharacterSize(20);
            entryTexts[i].setFillColor(sf::Color::White);
            entryTexts[i].setPosition(500, 60 + i * 30);
        }

        loadLeaderboard(); // Load from disk at the start
    }

    void loadLeaderboard() 
	{
        leaderboard.readFromFile(); // Simply wraps this call to the minheap
    }

    void displayLeaderboard(sf::RenderWindow& window) 
    {
        window.clear(sf::Color::Black);  // Clear screen with black
        draw(window);                    // Draw existing leaderboard data
        window.display();                // Display contents
    }

    void updateAfterMatch(int playerID, int playerScore) {
        leaderboard.insertOrUpdate(playerID, playerScore);
        leaderboard.writeToFile(); // Persist updated leaderboard
    }

    void draw(sf::RenderWindow& window) 
    {
        score top10[10];
        int count = leaderboard.getSortedDescending(top10);

        window.draw(titleText);
        for (int i = 0; i < count; ++i) {
            std::stringstream ss;
            ss << (i + 1) << ". Player " << top10[i].id << " - " << top10[i].pscore;
            entryTexts[i].setString(ss.str());
            window.draw(entryTexts[i]);
        }
    }
};
