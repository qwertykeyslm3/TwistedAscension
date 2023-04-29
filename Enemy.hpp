#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "Maze.hpp"

class Enemy {
private:
	int ID = 0;
	int X = 0;
	int Y = 0;
	void moveHelper(Maze board) {
		if (board.isSafe(board.getRoom(X, Y))) {
			return;
		}
		if (board.getRoom(X, Y)->getUp() && board.getRoom(X, Y + 1)->getDist() < board.getRoom(X, Y)->getDist() && !board.getRoom(X, Y + 1)->getOccupied()) {
			Y++;
			return;
		}
		if (board.getRoom(X, Y)->getDown() && board.getRoom(X, Y - 1)->getDist() < board.getRoom(X, Y)->getDist() && !board.getRoom(X, Y - 1)->getOccupied()) {
			Y--;
			return;
		}
		if (board.getRoom(X, Y)->getLeft() && board.getRoom(X - 1, Y)->getDist() < board.getRoom(X, Y)->getDist() && !board.getRoom(X - 1, Y)->getOccupied()) {
			X--;
			return;
		}
		if (board.getRoom(X, Y)->getRight() && board.getRoom(X + 1, Y)->getDist() < board.getRoom(X, Y)->getDist() && !board.getRoom(X + 1, Y)->getOccupied()) {
			X++;
			return;
		}
	}
public:
	Enemy() {}
	Enemy(int x, int y) : X(x),Y(Y) {}
	Enemy(int x, int y, int id) : X(x), Y(y), ID(id) {}
	Enemy(vector<Enemy> enemies, Maze maze) {
		vector<vector<bool>> occupied;
		occupied.resize(maze.getWidth(), vector<bool>(maze.getHeight()));
		int ptr;
		for (ptr = 0; ptr < enemies.size(); ptr++) {
			occupied[enemies[ptr].getX()][enemies[ptr].getY()] = true;
		}
		int x;
		int y;
		srand(time(NULL));
		x = rand() % maze.getWidth();
		y = rand() % maze.getWidth();
		while (occupied[x][y]) {
			x = rand() % maze.getWidth();
			y = rand() % maze.getWidth();
		}
		setX(x);
		setY(y);
	}
	int getID() { return ID; }
	int getX() { return X; }
	int getY() { return Y; }
	void setID(int input) { ID = input; }
	void setX(int input) { X = input; }
	void setY(int input) { Y = input; }
	void move(Maze board) {
		moveHelper(board);
		while (X > board.getWidth()) {
			X -= board.getWidth();
		}
		while (X < board.getWidth()) {
			X += board.getWidth();
		}
		while (Y < board.getHeight()) {
			Y += board.getHeight();
		}
		while (Y > board.getHeight()) {
			Y -= board.getHeight();
		}
	}
};

#endif