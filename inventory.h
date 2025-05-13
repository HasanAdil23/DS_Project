#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

// A Theme holds an ID key, name, description, and previewCode (e.g. texture path or color code).
struct Theme {
    int  id;
    string name;
    string description;
    string previewCode;
};

int max_int(int a, int b) { return (a > b ? a : b); }

// AVL tree node storing a Theme
struct AVLNode {
    Theme    data;
    AVLNode* left;
    AVLNode* right;
    int      height;
    AVLNode(const Theme& t)
        : data(t), left(nullptr), right(nullptr), height(1) {}
};

// Self‑balancing AVL Tree for Theme inventory
class AVLTree {
public:
    AVLTree() : root(nullptr) {}

    // Insert a new theme (unique id)
    void insert(const Theme& t) {
        root = insertNode(root, t);
    }

    // Find a theme by its ID, or return nullptr
    Theme* findByID(int id) const 
    {
        return findByIDNode(root, id);
    }

    // Export all themes into a dynamically allocated array; returns count.
    // Caller must delete[] the returned array.
    int toArray(Theme*& outArray) const {
        int count = countNodes(root);
        if (count == 0) { outArray = nullptr; return 0; }
        outArray = new Theme[count];
        int idx = 0;
        fillArray(root, outArray, idx);
        return count;
    }

private:
    AVLNode* root;

    int height(AVLNode* node) const {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode* node) const {
        return node ? height(node->left) - height(node->right) : 0;
    }

    // Right rotation
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = 1 + max_int(height(y->left), height(y->right));
        x->height = 1 + max_int(height(x->left), height(x->right));
        return x;
    }

    // Left rotation
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = 1 + max_int(height(x->left), height(x->right));
        y->height = 1 + max_int(height(y->left), height(y->right));
        return y;
    }

    // Insert helper
    AVLNode* insertNode(AVLNode* node, const Theme& t) {
        if (!node)
            return new AVLNode(t);
        if (t.id < node->data.id)
            node->left = insertNode(node->left, t);
        else if (t.id > node->data.id)
            node->right = insertNode(node->right, t);
        else
            return node; // duplicate IDs not allowed

        // Update height
        node->height = 1 + max_int(height(node->left), height(node->right));
        int balance = getBalance(node);

        // LL case
        if (balance > 1 && t.id < node->left->data.id)
            return rotateRight(node);
        // RR case
        if (balance < -1 && t.id > node->right->data.id)
            return rotateLeft(node);
        // LR case
        if (balance > 1 && t.id > node->left->data.id) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // RL case
        if (balance < -1 && t.id < node->right->data.id) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    // Count nodes in subtree
    int countNodes(AVLNode* node) const {
        if (!node) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

    // Fill array in-order
    void fillArray(AVLNode* node, Theme* arr, int& idx) const {
        if (!node) return;
        fillArray(node->left, arr, idx);
        arr[idx++] = node->data;
        fillArray(node->right, arr, idx);
    }

    // Search helper
    Theme* findByIDNode(AVLNode* node, int id) const
    {
        if (!node) 
            return nullptr;
        if (id == node->data.id)
            return &node->data;
        if (id < node->data.id)
            return findByIDNode(node->left, id);
        else
            return findByIDNode(node->right, id);
    }
};

// Load a preset list of Themes into the given tree
void loadDefaultThemes(AVLTree& tree) 
{
    tree.insert({ 1, "Classic",   "Default Enemy", "" });
    tree.insert({ 2, "Apocalypse",    "Droid Enemy",  "" });
    tree.insert({ 3, "Genesis", "Alien Enemy",           "" });
    tree.insert({ 4, "1984",    "Dystopian",     "" });
    
}


int showInventoryWindow(RenderWindow& window, AVLTree& tree) {

    Texture backgroundTexture;
    Sprite backgroundSprite; 
    Font font;
    if (!backgroundTexture.loadFromFile("images/t1.jpg"))
    {
        std::cerr << "Failed to load background image\n";
    }
    else 
    {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setColor(sf::Color(255, 255, 255, 100)); // Optional: transparency

        Vector2u textureSize = backgroundTexture.getSize();
        Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            float(windowSize.x) / textureSize.x,
            float(windowSize.y) / textureSize.y
        );
    }
    if (!font.loadFromFile("Fonts/super-legend-boy-font/SuperLegendBoy-4w8Y.ttf"))
    {
        std::cerr << "Failed to load font.\n";
        return -1;
    }

    Theme* themeList = nullptr;
    int themeCount = tree.toArray(themeList);
    if (themeCount == 0) return -1;

    int selectedIndex = 0;

    while (window.isOpen()) 
    {
        sf::Event e;
        while (window.pollEvent(e)) 
        {
            if (e.type == sf::Event::Closed)
                return -1;
            if (e.type == sf::Event::KeyPressed) 
            {
                if (e.key.code == sf::Keyboard::Up)
                    selectedIndex = (selectedIndex - 1 + themeCount) % themeCount;
                else if (e.key.code == sf::Keyboard::Down)
                    selectedIndex = (selectedIndex + 1) % themeCount;
                else if (e.key.code == sf::Keyboard::Enter) 
                {
                    int chosenID = themeList[selectedIndex].id;
                    delete[] themeList;
                    return chosenID;
                }
                else if (e.key.code == sf::Keyboard::Escape) {
                    delete[] themeList;
                    return -1;
                }
            }
        }

        window.clear(Color::Black);
        window.draw(backgroundSprite);
        for (int i = 0; i < themeCount; ++i) 
        {
            sf::Text text;
            text.setFont(font);
            std::string line = std::to_string(themeList[i].id) + ". " + themeList[i].name + " - " + themeList[i].description;
            text.setString(line);
            text.setCharacterSize(24);
            text.setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
            text.setPosition(60, 100 + i * 40);
            window.draw(text);
        }

        window.display();
    }

    delete[] themeList;
    return -1;
}