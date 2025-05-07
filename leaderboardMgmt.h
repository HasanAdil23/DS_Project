#pragma once
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;

const int maxsize = 10;

struct score {
    int id;
    int pscore;
    string name;
};

class minheap {
private:
    score heap[maxsize];
    int size;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent].pscore > heap[index].pscore) {
                swap(heap[parent], heap[index]);
                index = parent;
            }
            else break;
        }
    }

    void heapifyDown(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;
            if (left < size && heap[left].pscore < heap[smallest].pscore)
                smallest = left;
            if (right < size && heap[right].pscore < heap[smallest].pscore)
                smallest = right;
            if (smallest != index) {
                swap(heap[index], heap[smallest]);
                index = smallest;
            }
            else break;
        }
    }

public:
    minheap() : size(0) {}

    void insertScore(int id, int scoreVal, const string& name) {
        if (size < maxsize) {
            heap[size].id = id;
            heap[size].pscore = scoreVal;
            heap[size].name = name;
            heapifyUp(size);
            ++size;
        }
        else if (scoreVal > heap[0].pscore) {
            heap[0].id = id;
            heap[0].pscore = scoreVal;
            heap[0].name = name;
            heapifyDown(0);
        }
    }


    void writeToFile() {
        ofstream outFile("leaderboard.txt");
        if (!outFile.is_open()) return;
        for (int i = 0; i < size; ++i)
            outFile << heap[i].name << " " << heap[i].id << " " << heap[i].pscore << '\n';
    }

    void readFromFile() {
        ifstream file("leaderboard.txt");
        string name;
        int id, scoreVal;
        if (!file.is_open()) return;
        while (file >> name >> id >> scoreVal)
        {
            insertScore(id, scoreVal, name);

        }
    }

    int getSortedDescending(score* output) {
        score temp[maxsize];
        for (int i = 0; i < size; ++i) temp[i] = heap[i];
        for (int i = 0; i < size - 1; ++i) {
            int maxIdx = i;
            for (int j = i + 1; j < size; ++j) {
                if (temp[j].pscore > temp[maxIdx].pscore) maxIdx = j;
            }
            if (maxIdx != i) swap(temp[i], temp[maxIdx]);
        }
        for (int i = 0; i < size; ++i) output[i] = temp[i];
        return size;
    }

    void print()
    {
        cout << "Printing minheap" << endl;
        for (int i = 0; i < size; i++)
        {
            cout << heap[i].name << ' ' << heap[i].id << ' ' << heap[i].pscore << endl;
        }
    }
};

class LeaderboardManager {
private:
    minheap heap;
    Text titleText;
    Font font;
    Text entryTexts[10];
    Texture backgroundTexture;
    Sprite backgroundSprite;
public:
    LeaderboardManager(sf::RenderWindow& window) 
    {
        if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf"))
            cerr << "Failed to load font in LeaderboardManager" << endl;
        if (!backgroundTexture.loadFromFile("images/t1.jpg"))
            cerr << "Failed to load background image" << endl;

        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setColor(sf::Color(255, 255, 255, 100));  // Optional transparency

        // Scale background to fit window
        sf::Vector2u textureSize = backgroundTexture.getSize();
        sf::Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            float(windowSize.x) / textureSize.x,
            float(windowSize.y) / textureSize.y
        );

        heap.readFromFile();
        updateText();
    }

    // Call after a game ends
    void updateAfterMatch(int id, int scoreVal, const string& name) 
    {
        heap.insertScore(id, scoreVal, name);
        heap.writeToFile();
        updateText();
    }

    // Update SFML text objects
    void updateText() {
        score topScores[maxsize];
        int count = heap.getSortedDescending(topScores);
        
        titleText.setFont(font);
        titleText.setCharacterSize(28);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setString("LEADERBOARD");
        titleText.setPosition(215, 20);

        for (int i = 0; i < count; ++i) 
        {
            entryTexts[i].setFont(font);
            entryTexts[i].setCharacterSize(24);
            entryTexts[i].setFillColor(sf::Color::White);
            entryTexts[i].setPosition(100, 100 + i * 30);
            entryTexts[i].setString(to_string(i + 1) + ". " + topScores[i].name + " (ID:" + to_string(topScores[i].id) + ") " + to_string(topScores[i].pscore));
        }
        for (int i = count; i < maxsize; ++i)
            entryTexts[i].setString("");
    }

    void displayLeaderboard(sf::RenderWindow& window)
    {
        heap.print();
        bool back = false;
        while (window.isOpen() && !back) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) window.close();
                if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                    back = true;
            }

            window.clear();
            window.draw(backgroundSprite);
            window.draw(titleText);  
            for (int i = 0; i < maxsize; ++i)
                window.draw(entryTexts[i]);
            window.display();
        }
    }

};
