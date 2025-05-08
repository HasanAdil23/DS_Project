#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "SFML/Graphics.hpp"
using namespace std;
using namespace sf;

// Node to store each tile or enemy state in a linked list
struct tilenode {
    int x;          // x-coordinate of tile/enemy interaction
    int y;          // y-coordinate
    bool grid;      // false = enemy, true = tile
    tilenode* next;
};

//currently only the tile state is saved
//file names are playerIDs not not SAVEIDs. 
class GameState {
private:
    Font font;                // Font for prompt UI
    Texture backgroundTexture; // Background texture for prompt
    Sprite backgroundSprite;   // Sprite to display background
    float windowWidth = 40 * 18;
    int playerID;             // ID of the player
    tilenode* head;           // Head of the linked list
    long saveID;              // Unique save identifier

    void clearList() {
        tilenode* current = head;
        while (current) {
            tilenode* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
    }

public:
    GameState() : playerID(0), head(nullptr), saveID(0) {
        if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
            cerr << "Failed to load font for resume prompt\n";
        }
        if (backgroundTexture.loadFromFile("images/t1.jpg")) {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setColor(Color(255, 255, 255, 100));
        }
    }

    GameState(int id) : playerID(id), head(nullptr) {
        saveID = static_cast<long>(time(nullptr));
        if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf")) {
            cerr << "Failed to load font for resume prompt\n";
        }
        if (backgroundTexture.loadFromFile("images/t1.jpg")) {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setColor(Color(255, 255, 255, 100));
        }
    }

    ~GameState() {
        clearList();
    }

    void setPlayerID(int id) {
        playerID = id;
    }

    int getsaveID()
    {
		return saveID;
	}

    void addTile(int x, int y, bool isTile = true) {
        tilenode* node = new tilenode;
        node->x = x;
        node->y = y;
        node->grid = isTile;
        node->next = head;
        head = node;
    }

    void saveStateToFile() {
        saveID = static_cast<long>(time(nullptr));

        std::filesystem::create_directory("saves");

        char filename[64];
        sprintf_s(filename, "saves/%d.txt", playerID);
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open save file for writing\n";
            return;
        }
        else cout << "file opened in savestateToFile()" << endl;

        file << playerID << "\n";
        file << saveID << "\n";

        tilenode* curr = head;
        while (curr) {
            file << curr->x << ' ' << curr->y << ' ' << curr->grid << '\n';
            curr = curr->next;
        }
        file.close();
        cout << "Game saved with Save ID: " << saveID << endl;
    }

    bool loadStateFromFile(long id) {
        clearList();

        char filename[64];
        sprintf_s(filename, "saves/%ld.txt", id);
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Save file not found.\n";
            return false;
        }
        else cout << "File opened in loadstatefromfile()" << endl;

        file >> playerID;
        file >> saveID;

        int x, y, flag;
        while (file >> x >> y >> flag) {
            addTile(x, y, flag == 1);
        }
        file.close();
        cout << "Game loaded from Save ID: " << saveID << endl;
        return true;
    }
    bool hasSavedGame(int id) {
        char filename[64];
        sprintf_s(filename, "%d.txt", id);
        ifstream file(filename);
        return file.good();
    }

    void printList() 
    {
        tilenode* curr = head;
        while (curr) {
            cout << "(" << curr->x << "," << curr->y << ") grid=" << curr->grid << " -> ";
            curr = curr->next;
        }
        cout << "NULL\n";
    }

    bool promptResumeSFML(RenderWindow& window) 
    {
        Vector2u textureSize = backgroundTexture.getSize();
        Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            float(windowSize.x) / textureSize.x,
            float(windowSize.y) / textureSize.y
        );

        string inputText = "";
        Text prompt("Enter Save ID (-1 for new game):", font, 21);
        prompt.setPosition(50, 100);
        prompt.setFillColor(Color::Yellow);

        Text inputDisplay("", font, 30);
        inputDisplay.setPosition(100, 160);
        inputDisplay.setFillColor(Color::Yellow);

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) return false;
                if (event.type == Event::TextEntered) {
                    if ((event.text.unicode >= '0' && event.text.unicode <= '9') || event.text.unicode == '-') {
                        inputText += static_cast<char>(event.text.unicode);
                    }
                    else if (event.text.unicode == 8 && !inputText.empty()) {
                        inputText.pop_back();
                    }
                }
                else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                    if (!inputText.empty()) {
                        long id = stol(inputText);
                        if (id == -1) return false;
                        return loadStateFromFile(playerID);
                    }
                }
            }

            inputDisplay.setString(inputText);
            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(prompt);
            window.draw(inputDisplay);
            window.display();
        }
        return false;
    }

    tilenode* getHead() {
        return head;
    }
};
