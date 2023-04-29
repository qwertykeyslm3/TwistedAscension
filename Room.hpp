#ifndef ROOM_HPP
#define ROOM_HPP

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class Room { // The data structure representing a room in the maze
private:
    vector<int> intVals{ 0, 0, 0, 0, 0, 0 }; // ID, world X, world Y, board X, board Y, Dist from player
    vector<bool> boolVals{ false, false, false, false, false, false }; // Visited, Up, Down, Left, Right, Occupied
public:
    // Constructors
    Room() {}
    Room(int boardX, int boardY) { intVals[3] = boardX; intVals[4] = boardY; }
    Room(int boardX, int boardY, int worldX, int worldY) {
        intVals[3] = boardX;
        intVals[4] = boardY;
        intVals[1] = worldX;
        intVals[2] = worldY;
    }
    // End of Constructors
    // Accessors listed by name rather than designation
    bool getVisited() { return boolVals[0]; }
    bool getUp() { return boolVals[1]; }
    bool getDown() { return boolVals[2]; }
    bool getLeft() { return boolVals[3]; }
    bool getRight() { return boolVals[4]; }
    bool getOccupied() { return boolVals[5]; }
    int getID() { return intVals[0]; }
    int getWorldX() { return intVals[1]; }
    int getWorldY() { return intVals[2]; }
    int getBoardX() { return intVals[3]; }
    int getBoardY() { return intVals[4]; }
    int getDist() { return intVals[5]; }
    // End of accessors
    // Modifiers
    void setVisited(bool input) { boolVals[0] = input; }
    void setUp(bool input) { boolVals[1] = input; }
    void setDown(bool input) { boolVals[2] = input; }
    void setLeft(bool input) { boolVals[3] = input; }
    void setRight(bool input) { boolVals[4] = input; }
    void setOccupied(bool input) { boolVals[5] = input; }
    void updateID() {
        int ID = 0;
        if (getUp()) {
            ID += 1;
        }
        if (getDown()) {
            ID += 2;
        }
        if (getLeft()) {
            ID += 4;
        }
        if (getRight()) {
            ID += 8;
        }
        intVals[0] = ID;
    }
    void setWorldX(int input) { intVals[1] = input; }
    void setWorldY(int input) { intVals[2] = input; }
    void setBoardX(int input) { intVals[3] = input; }
    void setBoardY(int input) { intVals[4] = input; }
    void setDist(int input) { intVals[5] = input; }
    // End of Modifiers
    bool isNeighbour(Room* input) { // Determine if a room is a neighbour to this room
        if (input == nullptr) {
            return false;
        }
        if (input->getBoardX() == getBoardX() && input->getBoardY() == getBoardY()) {
            return false;
        }
        return (abs(getBoardX() - input->getBoardX()) + abs(getBoardY() - input->getBoardY()) == 1);
    } // Method isNeighbour
    void connect(Room* input) { // Connect two adjacent rooms
        if (!isNeighbour(input)) {
            return;
        }
        if (input->getBoardY() > getBoardY()) {
            input->setDown(true);
            setUp(true);
            return;
        }
        if (input->getBoardY() < getBoardY()) {
            input->setUp(true);
            setDown(true);
            return;
        }
        if (input->getBoardX() > getBoardX()) {
            input->setLeft(true);
            setRight(true);
            return;
        }
        input->setRight(true);
        setLeft(true);
    } // Method connect
    void disconnect(Room* input) { // disconnect two adjacent rooms
        if (!isNeighbour(input)) {
            return;
        }
        if (input->getBoardY() > getBoardY()) {
            input->setDown(false);
            setUp(false);
            return;
        }
        if (input->getBoardY() < getBoardY()) {
            input->setUp(false);
            setDown(false);
            return;
        }
        if (input->getBoardX() > getBoardX()) {
            input->setLeft(false);
            setRight(false);
            return;
        }
        input->setRight(false);
        setLeft(false);
    } // Method connect
    void swap(Room* input) { // Swap the data of two rooms
        Room storage;
        storage.setUp(getUp());
        storage.setDown(getDown());
        storage.setLeft(getLeft());
        storage.setRight(getRight());
        storage.setWorldX(getWorldX());
        storage.setWorldY(getWorldY());
        storage.setVisited(getVisited());
        storage.setOccupied(getOccupied());
        setUp(input->getUp());
        setDown(input->getDown());
        setLeft(input->getLeft());
        setRight(input->getRight());
        setWorldX(input->getWorldX());
        setWorldY(input->getWorldY());
        setVisited(input->getVisited());
        setOccupied(input->getOccupied());
        updateID();
        input->setUp(storage.getUp());
        input->setDown(storage.getDown());
        input->setLeft(storage.getLeft());
        input->setRight(storage.getRight());
        input->setWorldX(storage.getWorldX());
        input->setWorldY(storage.getWorldY());
        input->setVisited(storage.getVisited());
        input->setOccupied(storage.getOccupied());
        input->updateID();
    } // Method swap
    void reset(int x, int y) {
        setUp(false);
        setDown(false);
        setLeft(false);
        setRight(false);
        setVisited(false);
        setOccupied(false);
        setWorldX(x);
        setWorldY(y);
    } // Method reset
    void reset() {
        reset(getWorldX(), getWorldY());
    }
}; // Class Room

#endif