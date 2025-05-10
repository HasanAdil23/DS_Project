#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

struct FriendNode {
    string friendID;
    FriendNode* next;
    FriendNode(string id) : friendID(id), next(nullptr) {}
};

class Player {
public:
    string id;
    string name;
    FriendNode* friendListHead;
    FriendNode* pendingRequestsHead;

    Player(string pid = "", string pname = "") {
        id = pid;
        name = pname;
        friendListHead = nullptr;
        pendingRequestsHead = nullptr;
    }

    void addFriend(string fid) {
        if (!isFriend(fid)) {
            FriendNode* newNode = new FriendNode(fid);
            newNode->next = friendListHead;
            friendListHead = newNode;
        }
    }

    bool isFriend(string fid) {
        FriendNode* temp = friendListHead;
        while (temp) {
            if (temp->friendID == fid) return true;
            temp = temp->next;
        }
        return false;
    }

    void sendFriendRequest(Player* to) {
        if (to == nullptr || to->id == id || to->hasPendingRequest(id) || to->isFriend(id)) return;
        FriendNode* newNode = new FriendNode(id);
        newNode->next = to->pendingRequestsHead;
        to->pendingRequestsHead = newNode;
        cout << "Request sent from " << id << " to " << to->id << endl;
    }

    bool hasPendingRequest(string fid) {
        FriendNode* temp = pendingRequestsHead;
        while (temp) {
            if (temp->friendID == fid) return true;
            temp = temp->next;
        }
        return false;
    }

    void acceptFriendRequest(string requesterID) {
        if (!hasPendingRequest(requesterID)) return;

        FriendNode* curr = pendingRequestsHead;
        FriendNode* prev = nullptr;
        while (curr && curr->friendID != requesterID) {
            prev = curr;
            curr = curr->next;
        }
        if (curr) {
            if (prev) prev->next = curr->next;
            else pendingRequestsHead = curr->next;
            delete curr;
        }

        addFriend(requesterID);
    }

    void viewFriends(RenderWindow& window, Font& font)
    {
        Texture backgroundTexture;
        Sprite backgroundSprite;
        if (!backgroundTexture.loadFromFile("images/t1.jpg")) {
            cout << "Failed to load background image\n";
        }
        else {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setColor(sf::Color(255, 255, 255, 100)); // Optional: transparency

            //making it fit
            Vector2u textureSize = backgroundTexture.getSize();
            Vector2u windowSize = window.getSize();
            backgroundSprite.setScale(
                float(windowSize.x) / textureSize.x,
                float(windowSize.y) / textureSize.y
            );
        }

        Text header("Friends:", font, 24);
        header.setPosition(50, 50);
        header.setFillColor(Color::White);

        Text friendTexts[50];
        FriendNode* temp = friendListHead;
        int count = 0;
        int y = 100;
        while (temp && count < 50) {
            friendTexts[count].setFont(font);
            friendTexts[count].setString(temp->friendID);
            friendTexts[count].setCharacterSize(20);
            friendTexts[count].setFillColor(Color::Cyan);
            friendTexts[count].setPosition(60, y);
            y += 30;
            temp = temp->next;
            count++;
        }

        while (window.isOpen()) {
            Event e;
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed) window.close();
                if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
                    return;
            }
            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(header);
            for (int i = 0; i < count; i++) window.draw(friendTexts[i]);
            window.display();
        }
    }

    void viewPendingRequests(RenderWindow& window, Font& font)
    {
        Texture backgroundTexture;
        Sprite backgroundSprite;
        if (!backgroundTexture.loadFromFile("images/t1.jpg")) {
            cout << "Failed to load background image\n";
        }
        else {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setColor(sf::Color(255, 255, 255, 100)); // Optional: transparency

            //making it fit
            Vector2u textureSize = backgroundTexture.getSize();
            Vector2u windowSize = window.getSize();
            backgroundSprite.setScale(
                float(windowSize.x) / textureSize.x,
                float(windowSize.y) / textureSize.y
            );
        }

        Text header("Pending Requests:", font, 24);
        header.setPosition(50, 50);
        header.setFillColor(Color::White);

        Text requestTexts[50];
        FriendNode* temp = pendingRequestsHead;
        int count = 0;
        int y = 100;
        while (temp && count < 50)
        {
            cout << "Pending: " << temp->friendID << endl;

            requestTexts[count].setFont(font);
            requestTexts[count].setString(temp->friendID);
            requestTexts[count].setCharacterSize(20);
            requestTexts[count].setFillColor(Color::Yellow);
            requestTexts[count].setPosition(60, y);
            y += 30;
            temp = temp->next;
            count++;
        }

        while (window.isOpen()) {
            Event e;
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed) window.close();
                if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
                    return;
            }
            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(header);
            for (int i = 0; i < count; i++) window.draw(requestTexts[i]);
            window.display();
        }
    }
};

class PlayerManager {
private:
    Player** playerArray;
    int size, capacity;

    struct HashNode {
        string id;
        int index;
        HashNode* next;
        HashNode(string i, int idx) : id(i), index(idx), next(nullptr) {}
    };

    static const int HASH_SIZE = 101;
    HashNode* hashTable[HASH_SIZE];

    int hashFunction(string id) {
        int sum = 0;
        for (int i = 0; i < id.length(); i++) sum += id[i];
        return sum % HASH_SIZE;
    }

public:
    PlayerManager(int cap = 100) {
        capacity = cap;
        size = 0;
        playerArray = new Player * [capacity];
        for (int i = 0; i < capacity; i++) playerArray[i] = nullptr;
        for (int i = 0; i < HASH_SIZE; i++) hashTable[i] = nullptr;
    }

    ~PlayerManager() {
        for (int i = 0; i < size; i++) delete playerArray[i];
        delete[] playerArray;
        for (int i = 0; i < HASH_SIZE; i++) {
            HashNode* curr = hashTable[i];
            while (curr) {
                HashNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

    int addPlayer(string id, string name) {
        if (size >= capacity || findPlayer(id) != nullptr) return -1;
        Player* p = new Player(id, name);
        playerArray[size] = p;

        int hashIndex = hashFunction(id);
        HashNode* node = new HashNode(id, size);
        node->next = hashTable[hashIndex];
        hashTable[hashIndex] = node;

        return size++;
    }

    Player* findPlayer(string id) {
        int hashIndex = hashFunction(id);
        HashNode* curr = hashTable[hashIndex];
        while (curr) {
            if (curr->id == id) return playerArray[curr->index];
            curr = curr->next;
        }
        return nullptr;
    }

    void saveFriendData(const string& filename) {
        ofstream fout(filename);
        for (int i = 0; i < size; ++i) {
            fout << playerArray[i]->id;
            FriendNode* f = playerArray[i]->friendListHead;
            while (f) {
                fout << " " << f->friendID;
                f = f->next;
            }
            fout << "\n";
        }
        fout.close();

        cout << "Friend data saved to " << filename << endl;
    }

    void loadFriendData(const string& filename) {
        ifstream fin(filename);
        string line;
        while (getline(fin, line)) {
            string id = "", fid = "";
            int i = 0;
            while (i < line.length() && line[i] != ' ') id += line[i++];
            Player* p = findPlayer(id);
            while (i < line.length()) {
                while (i < line.length() && line[i] == ' ') i++;
                fid = "";
                while (i < line.length() && line[i] != ' ') fid += line[i++];
                if (p && !fid.empty()) p->addFriend(fid);
            }
        }
        fin.close();
    }

    void savePendingRequests(const string& filename) {
        ofstream fout(filename);
        for (int i = 0; i < size; ++i) {
            fout << playerArray[i]->id;
            FriendNode* f = playerArray[i]->pendingRequestsHead;
            while (f) {
                fout << " " << f->friendID;
                f = f->next;
            }
            fout << "\n";
        }
        fout.close();

        cout << "Friend data saved to " << filename << endl;
    }

    void loadPendingRequests(const string& filename) {
        ifstream fin(filename);
        string line;
        while (getline(fin, line)) {
            string id = "", fid = "";
            int i = 0;
            while (i < line.length() && line[i] != ' ') id += line[i++];
            Player* p = findPlayer(id);
            while (i < line.length()) {
                while (i < line.length() && line[i] == ' ') i++;
                fid = "";
                while (i < line.length() && line[i] != ' ') fid += line[i++];
                if (p && !fid.empty()) {
                    FriendNode* newNode = new FriendNode(fid);
                    newNode->next = p->pendingRequestsHead;
                    p->pendingRequestsHead = newNode;

                    cout << "Loaded pending request from " << fid << " to " << p->id << endl;
                }
            }
        }
        fin.close();
    }


};

void loadAllPlayers(PlayerManager& pm)
{
    ifstream fin("players.txt");
    string name, pass, id;

    while (fin >> name >> pass >> id)
    {
        pm.addPlayer(id, name);
    }

    fin.close();
}

void showFriendMenu(RenderWindow& window, PlayerManager& pm, Player* currentPlayer, Font& font) 
{
    Texture backgroundTexture;
    Sprite backgroundSprite;
    if (!backgroundTexture.loadFromFile("images/t1.jpg")) {
        cout << "Failed to load background image\n";
    }
    else {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setColor(sf::Color(255, 255, 255, 100)); // Optional: transparency

        //making it fit
        Vector2u textureSize = backgroundTexture.getSize();
        Vector2u windowSize = window.getSize();
        backgroundSprite.setScale(
            float(windowSize.x) / textureSize.x,
            float(windowSize.y) / textureSize.y
        );
    }

    Text title("Friend Menu", font, 30);
    title.setPosition(240, 50);
    title.setFillColor(Color::White);

    Text options[5];
    string labels[5] = {
        "View Friends",
        "View Pending Requests",
        "Send Friend Request",
        "Accept Friend Request",
        "Exit"
    };

    for (int i = 0; i < 5; i++) {
        options[i].setFont(font);
        options[i].setString(labels[i]);
        options[i].setCharacterSize(22);
        options[i].setPosition(150, 120 + i * 40);
    }

    string inputBuffer = "";
    Text inputText("", font, 20);
    inputText.setPosition(150, 360);
    inputText.setFillColor(Color::Green);

    int selectedIndex = 0;
    bool enteringID = false;
    int selectedOption = -1;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) return;
                if (enteringID) {
                    if (e.key.code == Keyboard::Enter) {
                        if (selectedOption == 2) {
                            Player* to = pm.findPlayer(inputBuffer);
                            if (to) currentPlayer->sendFriendRequest(to);
                        }
                        else if (selectedOption == 3) {
                            currentPlayer->acceptFriendRequest(inputBuffer);
                            Player* other = pm.findPlayer(inputBuffer);
                            if (other) other->addFriend(currentPlayer->id);
                        }
                        inputBuffer = "";
                        enteringID = false;
                        selectedOption = -1;
                    }
                }
                else {
                    if (e.key.code == Keyboard::Up) {
                        if (selectedIndex > 0) selectedIndex--;
                    }
                    else if (e.key.code == Keyboard::Down) {
                        if (selectedIndex < 4) selectedIndex++;
                    }
                    else if (e.key.code == Keyboard::Enter) {
                        if (selectedIndex == 0) currentPlayer->viewFriends(window, font);
                        else if (selectedIndex == 1) currentPlayer->viewPendingRequests(window, font);
                        else if (selectedIndex == 2 || selectedIndex == 3) {
                            selectedOption = selectedIndex;
                            inputBuffer = "";
                            enteringID = true;
                        }
                        else if (selectedIndex == 4) return;
                    }
                }
            }
            if (e.type == Event::TextEntered && enteringID) {
                char ch = static_cast<char>(e.text.unicode);
                if (ch >= 32 && ch <= 126 && inputBuffer.size() < 20) {
                    inputBuffer += ch;
                }
                else if (ch == 8 && !inputBuffer.empty()) {
                    inputBuffer.pop_back();
                }
            }
        }

        inputText.setString((enteringID ? ((selectedOption == 2) ? "Send request to: " : "Accept request from: ") + inputBuffer : ""));

        window.clear(Color::Black);
        window.draw(backgroundSprite);
        window.draw(title);

        for (int i = 0; i < 5; i++)
        {
            options[i].setFillColor(i == selectedIndex ? Color::Red : Color::Yellow);
            //window.draw(backgroundSprite);
            window.draw(options[i]);
        }

        if (enteringID)
        {
            window.draw(inputText);
            //window.draw(backgroundSprite);
        }
            
        window.display();

    }
}
