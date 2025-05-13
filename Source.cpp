#include <iostream>
#include <SFML/Graphics.hpp>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>
#include <climits>
#include <SFML/Audio.hpp>

using namespace std;


// Define constants for cell states
enum CellState {
    WALL,
    PATH,
    KEY,
    FRUIT,
    EMPTY
};


struct Cell {
    int x;
    int y;
    int distance; 
    bool visited;
    Cell* prev; // Pointer to the previous cell in the shortest path
    CellState state;
   
    Cell() : x(0), y(0), distance(INT_MAX), visited(false), prev(nullptr), state(WALL) {}
    Cell(int _x, int _y) : x(_x), y(_y), distance(INT_MAX), visited(false), prev(nullptr), state(WALL) {}
};

class Fruit {
public:
    int x;
    int y;
    int id;
    int score;
    std::string type; // Type of fruit (e.g., apple, banana, etc.)
    sf::Texture texture;
    sf::Sprite sprite;

    Fruit(int xx, int yy, const std::string& t, int idd, int s) : x(xx), y(yy), type(t), id(idd), score(s) {}

    // Function to load texture for the fruit
    bool loadTexture(const std::string& filename) {
        return texture.loadFromFile(filename);
    }

    // Function to draw the fruit on the window
    void draw(sf::RenderWindow& window, float cellSize) {
        sprite.setTexture(texture);
        sprite.setScale(cellSize / texture.getSize().x, cellSize / texture.getSize().y);
        sprite.setPosition(x * cellSize, y * cellSize);
        window.draw(sprite);
    }
    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
    
    int getX() {
        return x;
    }
    int getY() {
        return y;
    }

    void setX(int xx) {
        x = xx;
    }

    void setY(int yy) {
        y = yy;
    }

    std::string getName() {
        return type;
    }

    int getID() {
        return id;
    }

    int getScore() {
        return score;
    }
};


class Player {
public:
    bool hasKey;
    int x;
    int y;
    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    int score;

    Player(int startX, int startY, float cellSize) : x(startX), y(startY) {
        if (!playerTexture.loadFromFile("gamePlayer2.png")) {
            cout << "Failed to load player texture!" << endl;
        }
        playerSprite.setTexture(playerTexture);
        playerSprite.setScale(cellSize / playerTexture.getSize().x, cellSize / playerTexture.getSize().y);

        hasKey = false;
        score = 0;
    }


    void collectKey() {
        hasKey = true;
    }

    bool hasCollectedKey() const {
        return hasKey;
    }

    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void draw(sf::RenderWindow& window, float cellSize) {
        playerSprite.setPosition(x * cellSize, y * cellSize);
        window.draw(playerSprite);
    }

    int getX() const { return x; }
    int getY() const { return y; }
    sf::FloatRect getGlobalBounds() const {
        return playerSprite.getGlobalBounds();
    }

    void updateScore(int s) {
        score = score + s;
    }
};



class Treasure {
public:
    int x;
    int y;
    int id;
    int score;
    std::string type;
    sf::Texture texture;
    sf::Sprite sprite;


    Treasure(int xx, int yy, std::string ss, int iidd, int s) {
        x = xx;
        y = yy;
        type = ss;
        id = iidd;
        score = s;
    }

    std::string getType() const {
        return type;
    }

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    int getX() {
        return x;
    }

    int getY() {
        return y;
    }

    void setX(int xx) {
        x = xx;

    }
    void setY(int yy) {
        y = yy;
    }

    std::string getName()const {
        return type;
    }

    int getID()const {
        return id;
    }

    int getScore() {
        return score;
    }
};

// Define to manage the maze
class Maze {
public:
    int width;
    int height;
    Cell* cells;

    int openingX; // X-coordinate for the opening
    int openingY;

    int storeID;

   

    Maze(int _width, int _height) : width(_width), height(_height) {
        // Initialize maze with walls
        cells = new Cell[width * height];
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                cells[i * width + j] = Cell(j, i);
            }
        }

       
    }

    // Destructor to deallocate memory
    ~Maze() {
        delete[] cells;

     
    }

   

    void addKey(int x, int y) {
        cells[y * width + x].state = KEY;
    }

    bool isKeyCollected(const Player& player) const {
        return cells[openingY * width + openingX].state != KEY || player.hasCollectedKey();
    }

    void removeKey(int x, int y) {
        // Find and remove the key from the maze
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (cells[i * width + j].state == KEY && cells[i * width + j].x == x && cells[i * width + j].y == y) {
                    cells[i * width + j].state = PATH; // Change the state of the cell to PATH (remove the key)
                    return; // Exit the function after removing the key
                }
            }
        }
    }

    bool checkCollision(const Player& player, const Maze& maze) {
        return maze.cells[player.getY() * maze.width + player.getX()].state == KEY;
    }
    // Function to perform depth-first search maze generation
    void generateMaze() {
        srand(time(0)); // Seed the random number generator
        std::stack<Cell*> stack;
        Cell* currentCell = &cells[(rand() % height) * width + (rand() % width)];
        currentCell->visited = true;
        stack.push(currentCell);

        while (!stack.empty()) {
            currentCell = stack.top();
            stack.pop();
            vector<Cell*> neighbors = getUnvisitedNeighbors(currentCell);
            if (!neighbors.empty()) {
                Cell* nextCell = neighbors[rand() % neighbors.size()];
                carvePath(currentCell, nextCell);
                nextCell->visited = true;
                stack.push(currentCell);
                stack.push(nextCell);
            }
        }

        // Open a path from the starting point to the ending point
        cells[1 * width + 0].state = PATH; // Opening at the top-left corner
        cells[(height - 2) * width + (width - 1)].state = PATH; // Opening at the bottom-right corner

        // Ensure the cells adjacent to the openings are also PATH
        cells[1 * width + 1].state = PATH;
        cells[(height - 2) * width + (width - 2)].state = PATH;

        openingX = width - 1;
        openingY = height - 2;
    }

   
    bool isCollidingWithWalls(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            // Out of bounds
            return true;
        }
        return cells[y * width + x].state == WALL;
    }

   

   
    vector<Cell*> getUnvisitedNeighbors(Cell* cell) {
        vector<Cell*> neighbors;
        if (cell->x > 1 && !cells[cell->y * width + cell->x - 2].visited)
            neighbors.push_back(&cells[cell->y * width + cell->x - 2]);
        if (cell->x < width - 2 && !cells[cell->y * width + cell->x + 2].visited)
            neighbors.push_back(&cells[cell->y * width + cell->x + 2]);
        if (cell->y > 1 && !cells[(cell->y - 2) * width + cell->x].visited)
            neighbors.push_back(&cells[(cell->y - 2) * width + cell->x]);
        if (cell->y < height - 2 && !cells[(cell->y + 2) * width + cell->x].visited)
            neighbors.push_back(&cells[(cell->y + 2) * width + cell->x]);
        return neighbors;
    }

    // Function to carve a path between two cells
    void carvePath(Cell* cell1, Cell* cell2) {
        int dx = cell2->x - cell1->x;
        int dy = cell2->y - cell1->y;
        cells[(cell1->y + dy / 2) * width + (cell1->x + dx / 2)].state = PATH;
        cells[cell2->y * width + cell2->x].state = PATH;
    }

    // Function to draw the maze on an SFML window
    void drawMaze(sf::RenderWindow& window, float cellSize, const vector<Cell*>& shortestPath, sf::Font& font) {
        // Load the key texture
        sf::Texture keyTexture;
        if (!keyTexture.loadFromFile("gamekey.png")) {
            // Handle error if texture loading fails
            cout << "Failed to load key texture!" << endl;
        }

        // Load the fruit texture
        sf::Texture fruitTexture;
        if (!fruitTexture.loadFromFile("fruitsss.jpeg")) {
            // Handle error if texture loading fails
            cout << "Failed to load fruit texture!" << endl;
        }

        fruitTexture.setSmooth(true); // Enable smoothing for better quality
        fruitTexture.setRepeated(false);


        sf::Color maroonishColor(128, 0, 0);
        // Draw maze cells
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                sf::RectangleShape rect(sf::Vector2f(cellSize, cellSize));
                rect.setPosition(j * cellSize, i * cellSize);
                if (cells[i * width + j].state == WALL)
                    rect.setFillColor(sf::Color::Black);
                else
                    rect.setFillColor(maroonishColor);
                window.draw(rect);

                // Draw the key if the cell contains a key
                if (cells[i * width + j].state == KEY) {
                    sf::Sprite keySprite(keyTexture);
                    keySprite.setScale(cellSize / keyTexture.getSize().x, cellSize / keyTexture.getSize().y);
                    keySprite.setPosition(j * cellSize, i * cellSize);
                    window.draw(keySprite);
                }

             

                // Display distance if cell has been visited during shortest path calculation
                if (cells[i * width + j].visited && cells[i * width + j].distance != INT_MAX) {
                    sf::Text text(std::to_string(cells[i * width + j].distance), font, 12);
                    text.setFillColor(sf::Color::Red);
                    text.setPosition(j * cellSize + cellSize * 0.3, i * cellSize + cellSize * 0.3);
                    window.draw(text);
                }
            }
        }

    }


    int findShortestPath(int startX, int startY, int endX, int endY, vector<Cell*>& shortestPath) {
        // Reset distances and visited flags
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                cells[i * width + j].distance = INT_MAX;
                cells[i * width + j].visited = false;
                cells[i * width + j].prev = nullptr;
            }
        }

        // Create a min-heap (priority queue) for Dijkstra's algorithm
        auto compare = [](const Cell* a, const Cell* b) { return a->distance > b->distance; };
        priority_queue<Cell*, vector<Cell*>, decltype(compare)> pq(compare);

        // Start cell
        Cell* startCell = &cells[startY * width + startX];
        startCell->distance = 0;
        pq.push(startCell);

        // Dijkstra's algorithm
        while (!pq.empty()) {
            Cell* currentCell = pq.top();
            pq.pop();
            currentCell->visited = true;

            // Stop if destination cell is reached
            if (currentCell->x == endX && currentCell->y == endY)
                break;

            // Process neighbors
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0)
                        continue; // Skip the current cell
                    int newX = currentCell->x + dx;
                    int newY = currentCell->y + dy;
                    if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                        Cell* neighbor = &cells[newY * width + newX];
                        if (!neighbor->visited && neighbor->state != WALL) {
                            int newDistance = currentCell->distance + 1; // Assuming each cell has unit cost
                            if (newDistance < neighbor->distance) {
                                neighbor->distance = newDistance;
                                neighbor->prev = currentCell;
                                pq.push(neighbor);
                            }
                        }
                    }
                }
            }
        }

        // Reconstruct the shortest path
        shortestPath.clear();
        Cell* currentCell = &cells[endY * width + endX];
        int shortestPathDistance = currentCell->distance;
        while (currentCell != nullptr) {
            shortestPath.push_back(currentCell);
            currentCell = currentCell->prev;
        }

        // Return the shortest path distance
        return shortestPathDistance;
    }

};
// Class that represents an item in the inventory
class Item {
public:
    int id; 
    std::string name; 

    
    Item(int _id, const std::string& _name) : id(_id), name(_name) {}

    std::string getName() {
        return name;
    }
};


class AVLNode {
public:
    Item* item; 
    AVLNode* left; 
    AVLNode* right; 
    int height; 

    
    AVLNode(Item* _item) : item(_item), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
public:
    AVLNode* root; // Pointer to the root of the AVL tree

  
    int getHeight(AVLNode* node) {
        if (node == nullptr)
            return 0;
        return node->height;
    }

    int getBalanceFactor(AVLNode* node) {
        if (node == nullptr)
            return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

   
    void updateHeight(AVLNode* node) {
        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    }

   
    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

       
        x->right = y;
        y->left = T2;

     
        updateHeight(y);
        updateHeight(x);

        return x;
    }

  
    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

      
        y->left = x;
        x->right = T2;

        
        updateHeight(x);
        updateHeight(y);

        return y;
    }

  
    AVLNode* balance(AVLNode* node) {
       

        
        int balanceFactor = getBalanceFactor(node);

       
        if (balanceFactor > 1) {
            // Left-Left case
            if (getBalanceFactor(node->left) >= 0)
                return rightRotate(node);
            // Left-Right case
            else {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
        }
        
        else if (balanceFactor < -1) {
            // Right-Right case
            if (getBalanceFactor(node->right) <= 0)
                return leftRotate(node);
            // Right-Left case
            else {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }
        }

        // No rotation needed
        return node;
    }

    
    AVLNode* insertNode(AVLNode* node, Item* item) {
      
        if (node == nullptr)
            return new AVLNode(item);

        if (item->id < node->item->id)
            node->left = insertNode(node->left, item);
        else if (item->id > node->item->id)
            node->right = insertNode(node->right, item);
        else
            return node;

       
        updateHeight(node);

        
        return balance(node);
    }

  
    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;

       
        while (current->left != nullptr)
            current = current->left;

        return current;
    }

    bool searchNode(AVLNode* root, int data) {
        
        while (root != nullptr) {
            
           
            if (data == root->item->id) {
                return true;
            }
            
            else if (data < root->item->id) {
                root = root->left;
            }
           
            else {
                root = root->right;
            }
        }
        
        return false;
    }
  
    AVLNode* deleteNode(AVLNode* root, int id) {
       
        if (root == nullptr)
            return root;

      
        if (id < root->item->id)
            root->left = deleteNode(root->left, id);
        
        else if (id > root->item->id)
            root->right = deleteNode(root->right, id);
        
        else {
           
            if ((root->left == nullptr) || (root->right == nullptr)) {
                AVLNode* temp = root->left ? root->left : root->right;

                
                if (temp == nullptr) {
                    temp = root;
                    root = nullptr;
                }
                else // One child case
                    *root = *temp; 

                delete temp;
            }
            else {
                // Node with two children
                AVLNode* temp = minValueNode(root->right);

                root->item = temp->item;

                root->right = deleteNode(root->right, temp->item->id);
            }
        }

      
        if (root == nullptr)
            return root;

       
        updateHeight(root);

       
        return balance(root);
    }

    AVLNode* deleteLeftmostNode(AVLNode* root) {
        // If root is null, return null
        if (root == nullptr)
            return nullptr;

        // If root has no left child, it's the leftmost node
        if (root->left == nullptr) {
            AVLNode* rightChild = root->right;
            delete root;
            return rightChild; // Return the right child
        }

        // Recursively delete leftmost node in left subtree
        root->left = deleteLeftmostNode(root->left);

        // Update height and balance the tree
        updateHeight(root);
        return balance(root);
    }
   
    
    void inorderTraversalHelper(AVLNode* node, vector<Item*>& items) {
        if (node != nullptr) {
            inorderTraversalHelper(node->left, items);
            items.push_back(node->item);
            inorderTraversalHelper(node->right, items);
        }
    }

    void collectItems(AVLNode* node, vector<std::string>& items) const {
        if (node == nullptr) {
            return;
        }
        
        collectItems(node->left, items);
        items.push_back(node->item->getName()); 
        collectItems(node->right, items);
    }

public:
   
    AVLTree() : root(nullptr) {}

   
    ~AVLTree() {
       
    }

  
    void insert(Item* item) {
        root = insertNode(root, item);
    }

   
    void remove(int id) {
        root = deleteNode(root, id);
    }

   
    bool search(int id) {
        
        return false;
    }

   
    void display() {

        vector<Item*> items = inorderTraversal();

       
        cout << "AVL Tree Items:" << endl;
        for (Item* item : items) {
            cout << item->id << ":" << item->name << endl;
        }
    }

    AVLNode* getRoot() {
        if (root != NULL) {
            return root;
        }

    }

    vector<Item*> inorderTraversal() {
        vector<Item*> items;
        inorderTraversalHelper(root, items);
        return items;
    }
    
    std::vector<std::string> getItems() const {
        vector<std::string> items;
      
        collectItems(root, items);
        return items;
    }

    

};



bool checkCollision(const Player& player, const Maze& maze) {
    return maze.cells[player.getY() * maze.width + player.getX()].state == KEY;
}


class Menu {
public:
    sf::Font font;
    sf::Text title;
    sf::Text options[3]; // Adjust the number of options as needed
    sf::RenderWindow& window;
    bool isPaused;
    bool isResumed;
    bool showInventory = false;
    sf::SoundBuffer menuSelectBuffer;
    sf::Sound menuSelectSound;


    Menu(sf::RenderWindow& win) : window(win), isPaused(false), isResumed(false) {
        font.loadFromFile("french.ttf");

        title.setFont(font);
        title.setString("Main Menu");
        title.setCharacterSize(50);
        title.setFillColor(sf::Color::White);
        title.setPosition(200, 50);

        const std::string optionTexts[3] = { "Start Game", "Options", "Exit" }; // Adjust option texts as needed

        for (int i = 0; i < 3; i++) {
            options[i].setFont(font);
            options[i].setString(optionTexts[i]);
            options[i].setCharacterSize(30);
            options[i].setFillColor(sf::Color::White);
            options[i].setPosition(300, 200 + i * 100);
        }
        if (!menuSelectBuffer.loadFromFile("button_sound.mp3")) {
            cout << "Failed to load menu select sound!" << endl;
        }
       
        menuSelectSound.setBuffer(menuSelectBuffer);

    }

    int displayMenu(sf::RenderWindow& window) {
        sf::Font font;
        if (!font.loadFromFile("french.ttf")) {
            cout << "Failed to load font!" << endl;
            return -1; 
        }
        sf::Color maroonishColor(128, 0, 0);
        sf::Text title;
        title.setFont(font);
        title.setString("Main Menu");
        title.setCharacterSize(50);
        title.setFillColor(sf::Color::Red);
        title.setPosition(150, 20);

        const std::string optionTexts[3] = { "Start Game", "Options", "Exit" };

        sf::Text options[3];
        for (int i = 0; i < 3; i++) {
            options[i].setFont(font);
            options[i].setString(optionTexts[i]);
            options[i].setCharacterSize(30);
            options[i].setFillColor(sf::Color::Red);
            options[i].setPosition(150, 150 + i * 50);
        }

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background444.png")) {
            cout << "Failed to load background image!" << endl;
            return -1;
        }
        //backgroundTexture.create(21, 21);
        sf::Sprite background(backgroundTexture);

        int selectedOption = -1;

        while (window.isOpen() && selectedOption == -1) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    selectedOption = 2; // Exit if the window is closed
                }
                else if (event.type == sf::Event::MouseButtonPressed) {
                    // Check if the mouse clicked
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < 3; i++) {
                        if (options[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedOption = i;
                            menuSelectSound.play();
                            break;
                        }
                    }
                }
            }

            window.clear();
            window.draw(background);
            window.draw(title);
            for (int i = 0; i < 3; i++) {
                window.draw(options[i]);
            }

            window.display();
        }

        return selectedOption;
    }

    void showInventoryWindow(sf::RenderWindow& window, const AVLTree& inventory) {
       
        sf::RenderWindow inventoryWindow(sf::VideoMode(400, 300), "Inventory");
        sf::Font font;
        if (!font.loadFromFile("french.ttf")) {
            cout  << "Failed to load font!" << endl;
            return;
        }

       
        sf::Text inventoryText;
        inventoryText.setFont(font);
        inventoryText.setString("Inventory:");
        inventoryText.setCharacterSize(24);
        inventoryText.setFillColor(sf::Color::White);
        inventoryText.setPosition(10, 10);

        vector<std::string> items = inventory.getItems();
        vector<sf::Text> itemTexts;
        for (size_t i = 0; i < items.size(); ++i) {
            sf::Text itemText;
            itemText.setFont(font);
            itemText.setString("- " + items[i]);
            itemText.setCharacterSize(20);
            itemText.setFillColor(sf::Color::White);
            itemText.setPosition(10, 50 + i * 30);
            itemTexts.push_back(itemText);
        }

        while (inventoryWindow.isOpen()) {
            sf::Event event;
            while (inventoryWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    inventoryWindow.close();
                }
                else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::B) {
                      
                        inventoryWindow.close();
                    }
                }
            }

            inventoryWindow.clear();
            inventoryWindow.draw(inventoryText);
            for (const auto& itemText : itemTexts) {
                inventoryWindow.draw(itemText);
            }
            inventoryWindow.display();
        }
    }

    void midGameMenu(sf::RenderWindow& window, const AVLTree& inventory) {
        sf::Font font;
        if (!font.loadFromFile("french.ttf")) {
            cout << "Failed to load font!" << endl;
            return;
        }

        sf::Text title;
        title.setFont(font);
        title.setString("Pause Menu");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 100);


        vector<std::string> items = inventory.getItems();
        vector<sf::Text> itemTexts; 
        for (const auto& item : items) {
            sf::Text itemText;
            itemText.setFont(font);
            itemText.setString("- " + item);
            itemText.setCharacterSize(20);
            itemText.setFillColor(sf::Color::White);
            itemText.setPosition(100, 250 + itemTexts.size() * 30); 
            itemTexts.push_back(itemText);
        }

        
        sf::Text resumeText;
        resumeText.setFont(font);
        resumeText.setString("Resume (R)");
        resumeText.setCharacterSize(32);
        resumeText.setFillColor(sf::Color::White);
        resumeText.setPosition(window.getSize().x / 2 - resumeText.getGlobalBounds().width / 2, 200);

        sf::Text quitText;
        quitText.setFont(font);
        quitText.setString("Quit (Q)");
        quitText.setCharacterSize(32);
        quitText.setFillColor(sf::Color::White);
        quitText.setPosition(window.getSize().x / 2 - quitText.getGlobalBounds().width / 2, 350);

        sf::Text inventoryText;
        inventoryText.setFont(font);
        inventoryText.setString("Inventory (I)");
        inventoryText.setCharacterSize(32);
        inventoryText.setFillColor(sf::Color::White);
        inventoryText.setPosition(window.getSize().x / 2 - inventoryText.getGlobalBounds().width / 2, 275);

        sf::RectangleShape inventoryButton(sf::Vector2f(100, 50));
        inventoryButton.setFillColor(sf::Color::Blue);
        inventoryButton.setPosition(50, 550); 

        while (window.isOpen() && isPaused) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        menuSelectSound.play();
                        resumeGame();
                    }
                    else if (event.key.code == sf::Keyboard::Q) {
                        menuSelectSound.play();
                        window.close();
                    }
                    else if (event.key.code == sf::Keyboard::I) {
                       
                        menuSelectSound.play();
                        showInventory = !showInventory;
                        if (showInventory) {
                            showInventoryWindow(window, inventory); 
                        }
                    }
                
                }
                else if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                       
                        sf::FloatRect buttonBounds = inventoryButton.getGlobalBounds();
                        if (buttonBounds.contains(event.mouseButton.x, event.mouseButton.y)) {
                            
                            menuSelectSound.play();
                            showInventory = !showInventory;
                        }
                    }
                }
            }

            window.clear();
            window.draw(title);

            window.draw(quitText);
            window.draw(resumeText);
            window.draw(inventoryText);
          
            window.display();
        }
    }

    void showInstructions() {
        sf::RenderWindow window(sf::VideoMode(400, 300), "Game Instructions");
        sf::Font font;
        if (!font.loadFromFile("french.ttf")) {
            cout << "Error loading font!" << endl;
            return;
        }

        sf::Text text;
        text.setFont(font);
        text.setString("Game Instructions:\n\n- Use arrow keys to move the player.\n- Collect fruits while avoiding enemies.\n- Colliding with enemies reduces score.\n- Press 'Esc' to close this window.");
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::White);
        text.setPosition(20, 20);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                    window.close();
            }

            window.clear();
            window.draw(text);
            window.display();
        }
    }
  
    void pauseGame() {
        isPaused = true;
       
    }

   
    void resumeGame() {
        isPaused = false;
       
    }


    void draw() {
        window.clear(sf::Color::Black);

        window.draw(title);
        for (int i = 0; i < 3; i++) {
            window.draw(options[i]);
        }

        window.display();
    }

   
};

class Enemy {
private:
    int x, y; // Position of the enemy

public:
    Enemy(int startX, int startY) : x(startX), y(startY) {}

    // Function to move the enemy randomly
    void moveRandomly( Maze& maze) {
        int dx = 0, dy = 0;

        // Randomly select a direction
        int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right
        switch (direction) {
        case 0: // Up
            dy = -1;
            break;
        case 1: // Down
            dy = 1;
            break;
        case 2: // Left
            dx = -1;
            break;
        case 3: // Right
            dx = 1;
            break;
        }

        // Check if the new position is not colliding with maze walls
        while (maze.isCollidingWithWalls(x + dx, y + dy)) {
            // If colliding, choose a new random direction
            direction = rand() % 4;
            switch (direction) {
            case 0: // Up
                dy = -1;
                dx = 0;
                break;
            case 1: // Down
                dy = 1;
                dx = 0;
                break;
            case 2: // Left
                dx = -1;
                dy = 0;
                break;
            case 3: // Right
                dx = 1;
                dy = 0;
                break;
            }
        }

        // Move the enemy to the new valid position with adjusted speed
        x += dx;
        y += dy;
    }


    // Function to get the position of the enemy
    int getX() const { return x; }
    int getY() const { return y; }
};

class Antique {
private:
    int x;
    int y;
    std::string name;
    int score;
    sf::Texture texture;
    int id;

public:
    Antique(int xx, int yy, int s, string ss, int idd) : x(xx), y(yy), score(s), name(ss), id(idd) {}

    int getScore() const {
        return score;
    }

    const sf::Texture& getTexture() const {
        return texture;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    void setTexture(const std::string& texturePath) {
        if (!texture.loadFromFile(texturePath)) {
            cout << "Failed to load texture for Antique!" << endl;
        }
    }

    void setX(int xx) {
        x = xx;

    }

    void setY(int yy) {
        y = yy;
    }

    int getID() {
        return id;
    }

    std::string getName() {
        return name;
    }
};

class Timer {
private:
    int seconds; // Total duration of the timer in seconds
    bool running; // Indicates whether the timer is running
    sf::Clock clock; // SFML clock object for measuring time

public:
    Timer(int duration) : seconds(duration), running(false) {}

    void start() {
        running = true;
        clock.restart();
    }

    void stop() {
        running = false;
    }

    bool isRunning() const {
        return running;
    }

   

    int getTimeLeft() const {
        if (!running) return seconds;
        int elapsedSeconds = clock.getElapsedTime().asSeconds();
        return std::max(0, seconds - elapsedSeconds);
    }
};


int main() {
    int width = 21; 
    int height = 21; 
    float cellSize = 20.0f; 

    
    sf::RenderWindow window(sf::VideoMode(width * cellSize, height * cellSize), "SFML Maze");

    AVLTree inventory;
   
    sf::Font font;
    if (!font.loadFromFile("french.ttf")) {
      
        cout << "Failed to load font!" << endl;
        return 1; 
    }

    sf::Texture escapeTexture;
    if (!escapeTexture.loadFromFile("escape.png")) {
        // Handle error if texture loading fails
        cout << "Failed to load escape texture!" << endl;
        return 1; // Return error code
    }



  
    sf::Sprite escapeSprite(escapeTexture);
    escapeSprite.setScale(cellSize / escapeTexture.getSize().x, cellSize / escapeTexture.getSize().y);

    
    Player player(1, 1, cellSize); 

 
    Maze maze(width, height);
    maze.generateMaze();


    maze.addKey(5, 5);

    vector<Treasure> treasures;
    treasures.emplace_back(6, 5, "Gold", 21, 170);
    treasures.emplace_back(10, 15, "Diamond", 22, 350);
    treasures.emplace_back(18, 8, "Emerald", 23, 200);
    treasures.emplace_back(3, 10, "Ruby", 24, 300);
    treasures.emplace_back(14, 4, "Sapphire", 25,310);
    treasures.emplace_back(7, 18, "Pearl", 26, 250);
    treasures.emplace_back(17, 12, "Amethyst", 27, 167);
    treasures.emplace_back(8, 3, "Topaz", 28, 200);
    treasures.emplace_back(12, 17, "Opal", 29, 150);
    treasures.emplace_back(19, 10, "Jade", 30, 150);


    sf::Texture treasureTexture;
    if (!treasureTexture.loadFromFile("gameTreasure.png")) {
        // Handle error if texture loading fails
        cout << "Failed to load treasure texture!" << endl;
        return 1; // Return error code
    }

 
    vector<sf::Sprite> treasureSprites;
    for (auto& treasure : treasures) {
        int treasureX = treasure.getX();
        int treasureY = treasure.getY();

        // Ensure the treasure's position is within the maze bounds
        if (treasureX >= 0 && treasureX < 21 && treasureY >= 0 && treasureY < 21) {
            // Check if the treasure's position overlaps with any wall cells in the maze
            if (maze.isCollidingWithWalls(treasureX, treasureY)) {
                // If the treasure overlaps with a wall, find a new position within the maze
                bool validPosition = false;
                while (!validPosition) {
                    int newX = rand() % 21;
                    int newY = rand() % 21;
                    if (!maze.isCollidingWithWalls(newX, newY)) {
                        treasure.setX(newX);
                        treasure.setY(newY);
                        validPosition = true;
                    }
                }
            }

            // Calculate the position of the treasure sprite relative to the maze
            float spriteX = treasure.getX() * cellSize;
            float spriteY = treasure.getY() * cellSize;

            // Create and configure the treasure sprite
            sf::Sprite treasureSprite(treasureTexture);
            treasureSprite.setPosition(spriteX, spriteY);
            treasureSprite.setScale(cellSize / treasureTexture.getSize().x, cellSize / treasureTexture.getSize().y);

            // Add the treasure sprite to the vector of treasure sprites
            treasureSprites.push_back(treasureSprite);
        }
    }

    bool gameRunning = true;
    bool playerEscaped = false;
    bool pause = false;
  

    vector<Fruit> fruits;
    fruits.emplace_back(6, 5, "Apple", 11, 30);
    fruits.emplace_back(10, 15, "Banana", 12, 50);
    fruits.emplace_back(18, 8, "Orange", 13, 25);
    fruits.emplace_back(3, 10, "Grape", 14, 50);
    fruits.emplace_back(14, 4, "Strawberry", 15, 55);
    fruits.emplace_back(7, 18, "Pineapple", 16, 65);
    fruits.emplace_back(17, 12, "Watermelon", 17, 70);
    fruits.emplace_back(8, 3, "Cherry", 18, 80);
    fruits.emplace_back(12, 17, "Kiwi", 19, 90);
    fruits.emplace_back(19, 10, "Peach", 20, 100);

    sf::Texture fruitTexture;
    if (!fruitTexture.loadFromFile("gameFruit.png")) {
        // Handle error if texture loading fails
        cout << "Failed to load fruit texture!" << endl;
        return 1; // Return error code
    }

    vector<sf::Sprite> fruitSprites;
    for (auto& fruit : fruits) {
        int fruitX = fruit.getX();
        int fruitY = fruit.getY();

        // Ensure the fruit's position is within the maze bounds
        if (fruitX >= 0 && fruitX < 21 && fruitY >= 0 && fruitY < 21) {
            // Check if the fruit's position overlaps with any wall cells in the maze
            if (maze.isCollidingWithWalls(fruitX, fruitY)) {
                // If the fruit overlaps with a wall, find a new position within the maze
                bool validPosition = false;
                while (!validPosition) {
                    int newX = rand() % 21;
                    int newY = rand() % 21;
                    if (!maze.isCollidingWithWalls(newX, newY)) {
                        fruit.setX(newX);
                        fruit.setY(newY);
                        validPosition = true;
                    }
                }
            }

            // Calculate the position of the fruit sprite relative to the maze
            float spriteX = fruit.getX() * cellSize;
            float spriteY = fruit.getY() * cellSize;

            // Create and configure the fruit sprite
            sf::Sprite fruitSprite(fruitTexture);
            fruitSprite.setPosition(spriteX, spriteY);
            fruitSprite.setScale(cellSize / fruitTexture.getSize().x, cellSize / fruitTexture.getSize().y);

            // Add the fruit sprite to the vector of fruit sprites
            fruitSprites.push_back(fruitSprite);
        }
    }

    vector<Antique> antiques;
    antiques.emplace_back(6, 5, 50, "Enigmatic Relic", 1);
    antiques.emplace_back(10, 15, 30, "Cryptic Artifact", 2);
    antiques.emplace_back(18, 8, 20, "Mystical Ornament", 3);
    antiques.emplace_back(3, 10, 40, "Ethereal Trinket", 4);
    antiques.emplace_back(14, 4, 25, "Arcane Idol", 5);
    antiques.emplace_back(7, 18, 35, "Secret Talisman", 6);
    antiques.emplace_back(17, 12, 45, "Whispering Reliquary", 7);
    antiques.emplace_back(9, 11, 55, "Shadowy Artifact", 8);
    antiques.emplace_back(13, 6, 28, "Haunting Keepsake", 9);
    antiques.emplace_back(20, 2, 37, "Enchanted Emblem", 10);
  
    sf::Texture antiqueTexture;
    if (!antiqueTexture.loadFromFile("antique_texture.png")) {
        
        cout << "Failed to load antique texture!" << endl;
        return 1; 
    }

   
    vector<sf::Sprite> antiqueSprites;
    for (auto& antique : antiques) {
        int antiqueX = antique.getX();
        int antiqueY = antique.getY();

        // Ensure the antique's position is within the maze bounds
        if (antiqueX >= 0 && antiqueX < 21 && antiqueY >= 0 && antiqueY < 21) {
            // Check if the antique's position overlaps with any wall cells in the maze
            if (maze.isCollidingWithWalls(antiqueX, antiqueY)) {
                // If the antique overlaps with a wall, find a new position within the maze
                bool validPosition = false;
                while (!validPosition) {
                    int newX = rand() % 21;
                    int newY = rand() % 21;
                    if (!maze.isCollidingWithWalls(newX, newY)) {
                        antique.setX(newX);
                        antique.setY(newY);
                        validPosition = true;
                    }
                }
            }

            // Calculate the position of the antique sprite relative to the maze
            float spriteX = antique.getX() * cellSize;
            float spriteY = antique.getY() * cellSize;

            // Create and configure the antique sprite
            sf::Sprite antiqueSprite(antiqueTexture);
            antiqueSprite.setPosition(spriteX, spriteY);
            antiqueSprite.setScale(cellSize / antiqueTexture.getSize().x, cellSize / antiqueTexture.getSize().y);

            // Add the antique sprite to the vector of antique sprites
            antiqueSprites.push_back(antiqueSprite);
        }
    }

    vector<Enemy> enemies;
    enemies.emplace_back(3, 3);
    enemies.emplace_back(15, 10); // Additional enemy
    enemies.emplace_back(8, 15);
    enemies.emplace_back(1, 20);
    enemies.emplace_back(20, 5);

    sf::Texture enemyTexture;
    if (!enemyTexture.loadFromFile("playerEnemy.png")) {
        
        cout << "Failed to load enemy texture!" << endl;
        return 1; 
    }

    // Create enemy sprites
    vector<sf::Sprite> enemySprites;
    for (const auto& enemy : enemies) {
        sf::Sprite enemySprite(enemyTexture);
        enemySprite.setScale(cellSize / enemyTexture.getSize().x, cellSize / enemyTexture.getSize().y);
        enemySprites.push_back(enemySprite);
    }
    


    int enemyMovementDelay = 200;
    sf::Clock enemyMovementClock;

    vector<int> indicesToRemove;

    sf::SoundBuffer collectSoundBuffer;
    if (!collectSoundBuffer.loadFromFile("button_sound.mp3")) {
        cout << "Failed to load collect sound!" << endl;
       
    }
    sf::Sound collectSound;
    collectSound.setBuffer(collectSoundBuffer);

    sf::SoundBuffer ouch;
    if (!ouch.loadFromFile("hit2.mp3")) {
        cout << "Failed to load collect sound!" << endl;
       
    }
    sf::Sound hitByEnemy;
    hitByEnemy.setBuffer(ouch);

    /*sf::SoundBuffer eat;
    if (!eat.loadFromFile("hit3.mp3")) {
        std::cerr << "Failed to load collect sound!" << std::endl;  
    }
    sf::Sound eatFood;
    eatFood.setBuffer(eat);*/

    sf::SoundBuffer eat;
    if (!eat.loadFromFile("eatRewards.mp3")) {
        cout << "Failed to load collect sound!" << endl;
    }
    sf::Sound eatFood;
    eatFood.setBuffer(eat);

    sf::SoundBuffer win;
    if (!win.loadFromFile("winGameAnthem.mp3")) {
        cout << "Failed to load collect sound!" << endl;
    }
    sf::Sound winGame;
    winGame.setBuffer(win);

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("music2.mp3")) {
        cout << "Failed to load background music!" << endl;
      
    };

    backgroundMusic.setVolume(50); 

    backgroundMusic.setLoop(true); 

    backgroundMusic.play();



    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::Green);
    scoreText.setPosition(10, 10);

    Timer timer(60);
    timer.start();

    Menu menu(window);
    bool gameLoop = true;
    while (gameLoop == true) {
        int selectedOption = menu.displayMenu(window);

        if (selectedOption == 0) {
            while (window.isOpen() && gameRunning) {
                
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        gameRunning = false; // Stop the game loop
                    }


                    // Handle keyboard input 
                    if (event.type == sf::Event::KeyPressed) {
                        int dx = 0, dy = 0;


                        if (event.key.code == sf::Keyboard::Up) {
                            dy = -1;
                        }
                        else if (event.key.code == sf::Keyboard::Down) {
                            dy = 1;
                        }
                        else if (event.key.code == sf::Keyboard::Left) {
                            dx = -1;
                        }
                        else if (event.key.code == sf::Keyboard::Right) {
                            dx = 1;
                        }
                        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {

                            collectSound.play();
                            menu.pauseGame();

                            menu.midGameMenu(window, inventory);

                            if (!menu.isPaused) {
                                continue;
                            }
                        }
                        // Check collision with maze walls
                        if (!maze.isCollidingWithWalls(player.getX() + dx, player.getY() + dy)) {
                            // If no collision, move the player
                            player.move(dx, dy);
                        }
                    }


                    // Handle key collection
                    if (event.type == sf::Event::KeyPressed && maze.isKeyCollected(player)) {
                        // Check collision with the key
                        if (checkCollision(player, maze)) {
                            // Player collected the key, mark it as collected and remove the key from the maze
                            player.collectKey();
                            maze.removeKey(player.getX(), player.getY());
                        }
                    }


                }



                cout << "X :" << player.getX() << endl;
                cout << "Y :" << player.getY() << endl;


                if (enemyMovementClock.getElapsedTime().asMilliseconds() >= enemyMovementDelay) {

                    for (auto& enemy : enemies) {
                        enemy.moveRandomly(maze);
                    }


                    enemyMovementClock.restart();
                }



                for (const auto& enemy : enemies) {
                    if (player.getX() == enemy.getX() && player.getY() == enemy.getY()) {

                        hitByEnemy.play();


                        if (inventory.root != nullptr) {
                            // Delete the leftmost node in the inventory
                            inventory.root = inventory.deleteLeftmostNode(inventory.root);

                            player.updateScore(-100);
                            cout << "Item deleted from inventory." << endl;
                        }
                        else {
                            player.updateScore(-100);
                            cout << "Inventory is empty." << endl;
                        }
                    }
                }

                for (auto it = antiques.begin(); it != antiques.end();) {
                    eatFood.play();

                    if (player.getX() == it->getX() && player.getY() == it->getY()) {


                        player.updateScore(it->getScore());


                        std::string antiqueName = it->getName();
                        int antiqueId = it->getID();
                        Item* newItem = new Item(antiqueId, antiqueName);
                        inventory.insert(newItem);


                        it = antiques.erase(it);
                    }
                    else {

                        ++it;
                    }
                }

                for (auto it = treasures.begin(); it != treasures.end();) {
                    eatFood.play();

                    if (player.getX() == it->getX() && player.getY() == it->getY()) {

                        player.updateScore(it->getScore());


                        std::string treasureName = it->getName();
                        int treasureId = it->getID();
                        Item* newItem = new Item(treasureId, treasureName);
                        inventory.insert(newItem);


                        it = treasures.erase(it);
                    }
                    else {
                        ++it;
                    }
                }

                for (auto it = fruits.begin(); it != fruits.end();) {
                    eatFood.play();
                    if (player.getX() == it->getX() && player.getY() == it->getY()) {

                        player.updateScore(it->getScore());

                        std::string fruitName = it->getName();
                        int fruitId = it->getID();
                        Item* newItem = new Item(fruitId, fruitName);
                        inventory.insert(newItem);


                        it = fruits.erase(it);
                    }
                    else {

                        ++it;
                    }
                }


                scoreText.setString("Score: " + std::to_string(player.score));



                vector<Cell*> shortestPath;
                int shortestDistance = maze.findShortestPath(player.getX(), player.getY(), maze.openingX, maze.openingY, shortestPath);



                // Update the window
                window.clear();
                maze.drawMaze(window, cellSize, shortestPath, font);
                player.draw(window, cellSize);



                if (player.hasCollectedKey()) {
                    escapeSprite.setPosition(maze.openingX * cellSize, maze.openingY * cellSize);
                    window.draw(escapeSprite);
                }

                for (size_t i = 0; i < enemies.size(); ++i) {
                    enemySprites[i].setPosition(enemies[i].getX() * cellSize, enemies[i].getY() * cellSize);
                    window.draw(enemySprites[i]);
                }

                for (size_t i = 0; i < antiques.size(); ++i) {
                    antiqueSprites[i].setPosition(antiques[i].getX() * cellSize, antiques[i].getY() * cellSize);
                    window.draw(antiqueSprites[i]);
                }

                for (size_t i = 0; i < treasures.size(); ++i) {
                    treasureSprites[i].setPosition(treasures[i].getX() * cellSize, treasures[i].getY() * cellSize);
                    window.draw(treasureSprites[i]);
                }

                for (size_t i = 0; i < fruits.size(); ++i) {
                    fruitSprites[i].setPosition(fruits[i].getX() * cellSize, fruits[i].getY() * cellSize);
                    window.draw(fruitSprites[i]);
                }


                window.draw(scoreText);

                window.display();


                if (player.hasCollectedKey() && player.getX() == maze.openingX && player.getY() == maze.openingY) {

                    cout << "Congratulations! You escaped the maze!" << endl;

                    sf::Text escapeMessage;
                    escapeMessage.setFont(font);
                    escapeMessage.setString("Congratulations! You escaped the maze!");
                    escapeMessage.setCharacterSize(24);
                    escapeMessage.setFillColor(sf::Color::Green);
                    escapeMessage.setStyle(sf::Text::Bold);


                    sf::FloatRect textBounds = escapeMessage.getLocalBounds();
                    escapeMessage.setPosition((window.getSize().x - textBounds.width) / 2, (window.getSize().y - textBounds.height) / 2);

                    window.clear();
                    window.draw(escapeMessage);

                    window.display();
                    winGame.play();
                    sf::sleep(sf::seconds(3));
                    gameLoop = false;
                    window.close();
                }

                if (timer.isRunning()) {
                    int timeLeft = timer.getTimeLeft();
                    cout << "time left " <<timeLeft << endl;
                    if (timeLeft <= 0) {
                        sf::Text lostMessage;
                        lostMessage.setFont(font);
                        lostMessage.setString("Sorry! Times up!");
                        lostMessage.setCharacterSize(24);
                        lostMessage.setFillColor(sf::Color::Red);
                        lostMessage.setStyle(sf::Text::Bold);

                        sf::FloatRect textBounds = lostMessage.getLocalBounds();
                        lostMessage.setPosition((window.getSize().x - textBounds.width) / 2, (window.getSize().y - textBounds.height) / 2);

                        window.clear();
                        window.draw(lostMessage);

                        window.display();

                        sf::sleep(sf::seconds(2));
                        backgroundMusic.stop();
                        window.close();
                        gameLoop = false;
                        break;
                    }
                }

                if (player.score <= -500) {
                    cout << "Sorry! You Lost!" << endl;

                    sf::Text lostMessage;
                    lostMessage.setFont(font);
                    lostMessage.setString("Sorry! You Lost!");
                    lostMessage.setCharacterSize(24);
                    lostMessage.setFillColor(sf::Color::Red);
                    lostMessage.setStyle(sf::Text::Bold);

                    sf::FloatRect textBounds = lostMessage.getLocalBounds();
                    lostMessage.setPosition((window.getSize().x - textBounds.width) / 2, (window.getSize().y - textBounds.height) / 2);

                    window.clear();
                    window.draw(lostMessage);

                    window.display();

                    sf::sleep(sf::seconds(2));
                    backgroundMusic.stop();
                    window.close();
                    gameLoop = false;
                    break;
                }

            }
        }
        else if (selectedOption == 1) {
            //backgroundMusic.stop();
            menu.showInstructions();
            
        }
        else if (selectedOption == 2) {
            gameLoop = false;
            backgroundMusic.stop();
            window.close();
        }
    }
    backgroundMusic.stop();

    return 0;
}





