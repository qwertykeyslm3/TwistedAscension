#ifndef MAZE_HPP
#define MAZE_HPP

#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <queue>
#include "Room.hpp"

using namespace std;

class Maze {
private:
    vector<int> intVals{ 5, 5, 2, 2 }; // Width, Height, player X, player Y
    vector<vector<Room>> board;
    bool firstGen = true;

    void initialize() { // Initialize the maze object
        srand(time(NULL));
        board.resize(getWidth(), vector<Room>(getHeight()));
        intVals[2] = getWidth() / 2;
        intVals[3] = getHeight() / 2;
        int x;
        int y;
        for (x = 0; x < getWidth(); x++) {
            for (y = 0; y < getHeight(); y++) {
                board[x][y] = Room(x, y, x - intVals[2], y - intVals[3]);
            }
        }
    } // Method initialize
public:
    // Constructors
    Maze() { initialize(); }
    Maze(int size) { if (size > 5) { intVals[0] = size; intVals[1] = size; } initialize(); }
    Maze(int width, int height) { if (width > 5) { intVals[0] = width; } if (height > 5) { intVals[1] = height; } initialize(); }
    // End of constructors
    // Accessors
    int getWidth() { return intVals[0]; }
    int getHeight() { return intVals[1]; }
    int getPlayerX() { return intVals[2]; }
    int getPlayerY() { return intVals[3]; }
    int getPlayerWorldX() { return board[getPlayerX()][getPlayerY()].getWorldX(); }
    int getPlayerWorldY() { return board[getPlayerX()][getPlayerY()].getWorldY(); }
    vector<vector<Room>> getBoard() { return board; }
    Room* getRoom(int x, int y) { return &board[x][y]; }
    // End of Accessors
    vector<Room*> getUnvisitedNeighbours(Room* input) {
        vector<Room*> output;
        if (input == nullptr) {
            return output;
        }
        return getUnvisitedNeighbours(input->getBoardX(), input->getBoardY());
    } // Method getUnvisitedNeighbours
    vector<Room*> getUnvisitedNeighbours(int x, int y) { // Get a list of unvisited neighbours of a room
        Room* current = getRoom(x, y);
        vector<Room*> output;
        if (y < getHeight() - 1 && !board[x][y + 1].getVisited()) {
            output.push_back(getRoom(x, y + 1));
        }
        if (y > 0 && !board[x][y - 1].getVisited()) {
            output.push_back(getRoom(x, y - 1));
        }
        if (x > 0 && !board[x - 1][y].getVisited()) {
            output.push_back(getRoom(x - 1, y));
        }
        if (x < getWidth() - 1 && !board[x + 1][y].getVisited()) {
            output.push_back(getRoom(x + 1, y));
        }
        return output;
    } // Method getUnvisitedNeighbours
    vector<Room*> getVisited() { // Get a list of all visited rooms on the board
        vector<Room*> output;
        int x;
        int y;
        for (x = 0; x < getWidth(); x++) {
            for (y = 0; y < getHeight(); y++) {
                Room* current = getRoom(x, y);
                if (current->getVisited()) {
                    output.push_back(current);
                }
            }
        }
        return output;
    } // Method getVisited
    vector<Room*> getUnvisited() { // Get a list of all unvisited rooms on the board
        vector<Room*> output;
        int x;
        int y;
        for (x = 0; x < getWidth(); x++) {
            for (y = 0; y < getHeight(); y++) {
                Room* current = getRoom(x, y);
                if (!current->getVisited()) {
                    output.push_back(current);
                }
            }
        }
        return output;
    } // Method getUnvisited
    vector<Room*> getActive() { // Get a list of the active rooms on the board
        vector<Room*> input = getVisited();
        vector<Room*> output;
        if (input.size() == 0) {
            Room* current = getRoom(getPlayerX(), getPlayerY());
            current->setVisited(true);
            output.push_back(current);
            return output;
        }
        int count;
        for (count = 0; count < input.size(); count++) {
            if (getUnvisitedNeighbours(input[count]).size() != 0) {
                output.push_back(input[count]);
            }
        }
        return output;
    } // Method getActive
    void generateMaze() { generateMaze(2); } // Default maze generation
    void generateMaze(int seed) { // Generate the actual maze
        if (seed < 1) {
            return;
        }
        vector<Room*> active = getActive();
        while (active.size() > 0) {
            int index = rand() % active.size();
            Room* current = active[index];
            vector<Room*> activeNeighbours = getUnvisitedNeighbours(current);
            int ptr;
            if (!firstGen && isSafe(current)) {
                active.erase(active.begin() + index);
            }
            else {
                for (ptr = 0; ptr < activeNeighbours.size(); ptr++) {
                    if (rand() % seed == 0) {
                        current->connect(activeNeighbours[ptr]);
                        activeNeighbours[ptr]->connect(current);
                        activeNeighbours[ptr]->setVisited(true);
                        if (getUnvisitedNeighbours(activeNeighbours[ptr]).size() > 0) {
                            active.push_back(activeNeighbours[ptr]);
                        }
                    }
                }
                if (getUnvisitedNeighbours(current).size() == 0) {
                    active.erase(active.begin() + index);
                }
            }
        }
        int x;
        int y;
        for (x = 0; x < getWidth(); x++) {
            for (y = 0; y < getHeight(); y++) {
                board[x][y].updateID();
            }
        }
        firstGen = false;
        updateDist();
    } // Method generateMaze
    void printBoard() { // Print the board in the console
        int y;
        int x;
        for (y = getHeight() - 1; y >= 0; y--) {
            for (x = 0; x < getWidth(); x++) {
                if (board[x][y].getUp()) {
                    cout << "+   ";
                }
                else {
                    cout << "+---";
                }
            }
            cout << "+";
            cout << endl;
            for (x = 0; x < getWidth(); x++) {
                if (board[x][y].getLeft()) {
                    cout << " ";
                }
                else {
                    cout << "|";
                }
                if (x == getPlayerX() && y == getPlayerY()) {
                    cout << " @ ";
                }
                else {
                    cout << "   ";
                }
            }
            if (board[x - 1][y].getRight()) {
                cout << " ";
            }
            else {
                cout << "|";
            }
            cout << endl;
        }
        for (x = 0; x < getWidth(); x++) {
            if (board[x][0].getDown()) {
                cout << "+   ";
            }
            else {
                cout << "+---";
            }
        }
        cout << "+";
        cout << endl;
    } // method printBoard
    void regen() { regen(2); } // Default regen method
    void regen(int seed) { // Regenerate maze based on a degeneration technique
        degen();
        generateMaze(seed);
    } // Method regen
    vector<Room*> getConnectedNeighbours(Room* input) {
        vector<Room*> output;
        int x = input->getBoardX();
        int y = input->getBoardY();
        if (y < getHeight() - 1 && input->getUp()) {
            output.push_back(&board[x][y + 1]);
        }
        if (y > 0 && input->getDown()) {
            output.push_back(&board[x][y - 1]);
        }
        if (x > 0 && input->getLeft()) {
            output.push_back(&board[x - 1][y]);
        }
        if (x < getWidth() - 1 && input->getRight()) {
            output.push_back(&board[x + 1][y]);
        }
        return output;
    } // Method getConnectedNeighbours
    bool isSafe(Room* input) { // Determine if a room is in the safe zone
        if (abs(input->getBoardX() - getPlayerX()) < 2 && abs(input->getBoardY() - getPlayerY()) < 2) {
            return true;
        }
        if (input->getBoardX() != getPlayerX() && input->getBoardY() != getPlayerY()) {
            return false;
        }
        Room* ptr = input;
        while (ptr->getBoardY() > getPlayerY()) {
            if (!ptr->getDown()) {
                return false;
            }
            ptr = getRoom(ptr->getBoardX(), ptr->getBoardY() - 1);
        }
        while (ptr->getBoardY() < getPlayerY()) {
            if (!ptr->getUp()) {
                return false;
            }
            ptr = getRoom(ptr->getBoardX(), ptr->getBoardY() + 1);
        }
        while (ptr->getBoardX() > getPlayerX()) {
            if (!ptr->getLeft()) {
                return false;
            }
            ptr = getRoom(ptr->getBoardX() - 1, ptr->getBoardY());
        }
        while (ptr->getBoardX() < getPlayerX()) {
            if (!ptr->getRight()) {
                return false;
            }
            ptr = getRoom(ptr->getBoardX() + 1, ptr->getBoardY());
        }
        return true;
    } // Method isSafe
    void degen() { // Degenerate the maze given specific rules about what areas are safe;
        int x;
        int y;
        int ptr;
        for (x = 0; x < getWidth(); x++) {
            for (y = 0; y < getHeight(); y++) {
                Room* current = getRoom(x, y);
                current->setVisited(false);
                vector<Room*> neighbours = getConnectedNeighbours(current);
                for (ptr = 0; ptr < neighbours.size(); ptr++) {
                    if (!isSafe(neighbours[ptr]) && !isSafe(current)) {
                        current->disconnect(neighbours[ptr]);
                    }
                    else {
                        current->setVisited(true);
                    }
                }
            }
        }

    } // Method degen
    void moveUp() { // Move every room down if the player can move up
        if (!board[getPlayerX()][getPlayerY()].getUp())
            return;
        board[getPlayerX()][getPlayerY()].setOccupied(false);
        int x;
        int y;
        for (y = 0; y < getHeight() - 1; y++) {
            for (x = 0; x < getWidth(); x++) {
                board[x][y].swap(getRoom(x, y + 1));
            }
        }
        for (x = 0; x < getWidth(); x++) {
            board[x][0].setDown(false);
            board[x][getHeight() - 2].setUp(false);
            board[x][getHeight() - 1].reset(board[x][getHeight() - 1].getWorldX(), board[x][getHeight() - 2].getWorldY() + 1);
        }
        board[getPlayerX()][getPlayerY()].setOccupied(true);
        regen();
    } // Method moveUp
    void moveDown() { // Move every room up if the player can move down
        if (!board[getPlayerX()][getPlayerY()].getDown())
            return;
        board[getPlayerX()][getPlayerY()].setOccupied(false);
        int x;
        int y;
        for (y = getHeight() - 1; y > 0; y--) {
            for (x = 0; x < getWidth(); x++) {
                board[x][y].swap(getRoom(x, y - 1));
            }
        }
        for (x = 0; x < getWidth(); x++) {
            board[x][1].setDown(false);
            board[x][getHeight() - 1].setUp(false);
            board[x][0].reset(board[x][1].getWorldX(), board[x][1].getWorldY() - 1);
        }
        board[getPlayerX()][getPlayerY()].setOccupied(true);
        regen();
    } // Method moveDown
    void moveLeft() { // Move every room right if the player can move left
        if (!board[getPlayerX()][getPlayerY()].getLeft())
            return;
        board[getPlayerX()][getPlayerY()].setOccupied(false);
        int x;
        int y;
        for (x = getWidth() - 1; x > 0; x--) {
            for (y = 0; y < getHeight(); y++) {
                board[x][y].swap(getRoom(x - 1, y));
            }
        }
        for (y = 0; y < getHeight(); y++) {
            board[getWidth() - 1][y].setRight(false);
            board[1][y].setLeft(false);
            board[0][y].reset(board[1][y].getWorldX() - 1, board[1][y].getWorldY());
        }
        board[getPlayerX()][getPlayerY()].setOccupied(true);
        regen();
    } // Method moveLeft
    void moveRight() { // Move every room left if the player can move right
        if (!board[getPlayerX()][getPlayerY()].getRight())
            return;
        board[getPlayerX()][getPlayerY()].setOccupied(false);
        int x;
        int y;
        for (x = 0; x < getWidth() - 1; x++) {
            for (y = 0; y < getHeight(); y++) {
                board[x][y].swap(getRoom(x + 1, y));
            }
        }
        for (y = 0; y < getHeight(); y++) {
            board[0][y].setLeft(false);
            board[getWidth() - 2][y].setRight(false);
            board[getWidth() - 1][y].reset(board[getWidth() - 2][y].getWorldX() + 1, board[getWidth() - 2][y].getWorldY());
        }
        board[getPlayerX()][getPlayerY()].setOccupied(true);
        regen();
    } // Method moveRight

    // The following method I translated from another class
    void updateDist() { // Using the BFS algorithm, go through the maze and update the distances of each room
        vector<vector<bool>> visited(getWidth(), vector<bool>(getHeight()));
        queue<pair<Room*, int>> active;
        visited[getPlayerX()][getPlayerY()] = true;
        active.push({ getRoom(getPlayerX(), getPlayerY()), 0 });
        while (active.size() > 0) {
            Room* next = active.front().first;
            int dist = active.front().second;
            next->setDist(dist);
            active.pop();
            queue<Room*> connected;
            if (next -> getUp()) {
                connected.push(getRoom(next->getBoardX(), next -> getBoardY() + 1));
            }
            if (next->getDown()) {
                connected.push(getRoom(next -> getBoardX(), next -> getBoardY() - 1));
            }
            if (next->getLeft()) {
                connected.push(getRoom(next -> getBoardX() - 1, next -> getBoardY()));
            }
            if (next->getRight()) {
                connected.push(getRoom(next -> getBoardX() + 1, next -> getBoardY()));
            }
            while (connected.size() > 0) {
                Room* n = connected.front();
                connected.pop();
                if (!visited[n -> getBoardX()][n -> getBoardY()]) {
                    visited[n->getBoardX()][n->getBoardY()] = true;
                    active.push({ n, dist + 1 });
                }
            }
            dist++;
        }
    } // Method updateDist
    void setFirstGen(bool input) {
        firstGen = input;
    }
};

#endif