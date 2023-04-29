#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "Maze.hpp"
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include "Enemy.hpp"

using namespace std;
using namespace sf;
using json = nlohmann::json;

void render();
void update();
int exit(string input, int error);

int fps = 60;
Maze maze(17);
int tileSize = 16;
int scaleFactor = 3;
string title = "Twisted Ascension";
int width = tileSize * scaleFactor * maze.getWidth();
int height = tileSize * scaleFactor * maze.getHeight();
RenderWindow window(VideoMode(width, height), title);
RenderTexture rt;
bool playing = true;
auto start_time = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch());
auto current_time = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch());
long long last_update = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
string levelsDataAddress = "assets/levelData.txt";
vector<json> levels;
int currentLevel = 0;
bool up;
bool down;
bool Left;
bool Right;
bool space;
int blink;
bool esc;
int pressCount = 0;
bool pause;
int enemyCount = 5;
long updateCount = 0;
vector<Enemy> enemies;

int main() {
	srand(start_time.count());
	maze.generateMaze();
	ifstream levelAddress(levelsDataAddress);
	if (!levelAddress.is_open()) {
		return exit("Levels not found. Closing.", 1);
	}
	string address;
	while (getline(levelAddress, address)) {
		ifstream nextLevel(address);
		json data;
		nextLevel >> data;
		levels.push_back(data);
		nextLevel.close();
	}
	levelAddress.close();
	if (levels.size() == 0) {
		return exit("no levels present. Closing.", 1);
	}
	while (playing) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				return exit("Window closed. Closing", 0);
			}
			if (event.type == Event::KeyPressed) {
				pressCount++;
				if (pressCount == 0) {
					if (event.key.code == Keyboard::W) up = true;
					if (event.key.code == Keyboard::A) down = true;
					if (event.key.code == Keyboard::S) Left = true;
					if (event.key.code == Keyboard::D) Right = true;
					if (event.key.code == Keyboard::Space) space = true;
					if (event.key.code == Keyboard::Escape) esc = true;
				}
			}
			if (event.type == Event::KeyReleased) {
				pressCount--;
			}
		}
		current_time = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch());
		if (current_time.count() - last_update >= 10000) {
			return exit("Updates fell behind by at least 10 seconds. Closing.", 1);
		}
		while (current_time.count() - last_update >= 1000 / fps) {
			last_update += (1000 / fps);
			update();
			updateCount++;
		}
		render();
	}
}

int exit(string input, int error) {
	cout << input;
	cout << endl;
	window.close();
	playing = false;
	return error;
}

void render() {
	Texture tileMap;
	Texture playerSprite;
	Texture enemyMap;
	tileMap.loadFromFile(levels[currentLevel]["tileAddress"]);
	playerSprite.loadFromFile(levels[currentLevel]["playerAddress"]);
	enemyMap.loadFromFile(levels[currentLevel]["enemyAddress"]);
	rt.create(width, height);
	rt.clear(Color::Black);

	int x;
	int y;

	Sprite tile;
	tile.setTexture(tileMap);
	tile.setScale(Vector2f(scaleFactor, scaleFactor));

	for (x = 0; x < maze.getWidth(); x++) {
		for (y = 0; y < maze.getHeight(); y++) {
			tile.setTextureRect(IntRect(maze.getRoom(x, y)->getID() * tileSize, 0, tileSize, tileSize));
			tile.setPosition(x * scaleFactor * tileSize, (height - (y * scaleFactor * tileSize)) - (tileSize * scaleFactor));
			if (blink <= 0) {
				rt.draw(tile);
			}
		}
	}

	Sprite player;
	player.setTexture(playerSprite);
	player.setScale(Vector2f(scaleFactor, scaleFactor));
	player.setPosition(maze.getPlayerX() * scaleFactor * tileSize, (height - (maze.getPlayerY() * scaleFactor * tileSize)) - (tileSize * scaleFactor));
	rt.draw(player);

	Sprite enemy;
	enemy.setTexture(enemyMap);
	enemy.setScale(Vector2f(scaleFactor, scaleFactor));
	int ptr = 0;
	while (ptr < enemies.size()) {
		enemy.setPosition(enemies[ptr].getX() * scaleFactor * tileSize, (height - (enemies[ptr].getY() * scaleFactor * tileSize)) - (tileSize * scaleFactor));
		enemy.setTextureRect(IntRect((enemies[ptr].getID()) * tileSize, 0, tileSize, tileSize));
		if (blink <= 0) {
			rt.draw(enemy);
		}
		ptr++;
	}

	rt.display();
	window.clear();
	window.draw(Sprite(rt.getTexture()));
	window.display();
}

void update() {
	while (enemies.size() < enemyCount) {
		enemies.push_back(Enemy(enemies, maze));
	}
	while (enemies.size() > enemyCount) {
		enemies.erase(enemies.begin() + (rand() % enemies.size()));
	}
	if (pause) {
		return;
	}
	if (blink > 0) {
		blink--;
		return;
	}
	if (space) {
		space = false;
		blink = 60;
		return;
	}
	if (up) {
		up = false;
		maze.moveUp();
		return;
	}
	if (down) {
		down = false;
		maze.moveDown();
		return;
	}
	if (Left) {
		Left = false;
		maze.moveLeft();
		return;
	}
	if (Right) {
		Right = false;
		maze.moveRight();
		return;
	}
	if (esc) {
		pause = !pause;
		esc = false;
	}
}