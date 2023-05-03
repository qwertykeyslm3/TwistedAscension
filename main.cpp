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
void nextLevel();
void randomizeEnemies();
string titleBuilder();
string playTime();
void collision();
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
bool space;
int blink;
bool Up;
bool Down;
bool Left;
bool Right;
bool esc;
bool pause;
int enemyCount = 5;
long updateCount = 0;
bool moveWithPlayer = false;
int mazeUpdateTime = 120;
int playerMoveTime = 0;
int moveTime = 5;
vector<Enemy> enemies;
int offsetMax = 50;
int blinkTime = 60;

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
				if (event.key.code == Keyboard::W) Up = true;
				if (event.key.code == Keyboard::A) Left = true;
				if (event.key.code == Keyboard::S) Down = true;
				if (event.key.code == Keyboard::D) Right = true;
				if (event.key.code == Keyboard::Space) space = true;
				if (event.key.code == Keyboard::Escape) esc = true;
			}
		}
		current_time = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch());
		if (current_time.count() - last_update >= 10000) {
			return exit("Updates fell behind by at least 10 seconds. Closing.", 1);
		}
		while (current_time.count() - last_update >= 1000 / fps) {
			last_update += (1000 / fps);
			update();
			if (!playing) {
				return 0;
			}
			updateCount++;
		}
		window.setTitle(titleBuilder());
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
			Color opacity = tile.getColor();
			opacity.a = 255;
			tile.setColor(opacity);
			if (blink <= 0) {
				if (!maze.isSafe(maze.getRoom(x, y))) {
					opacity.a = 64;
					tile.setColor(opacity);
					if (y + 1 < maze.getHeight() && maze.isSafe(maze.getRoom(x, y + 1)) && maze.getRoom(x, y)->getUp()) {
						opacity.a = 255;
						tile.setColor(opacity);
					}
					if (y - 1 >= 0 && maze.isSafe(maze.getRoom(x, y - 1)) && maze.getRoom(x, y)->getDown()) {
						opacity.a = 255;
						tile.setColor(opacity);
					}
					if (x + 1 < maze.getWidth() && maze.isSafe(maze.getRoom(x + 1, y)) && maze.getRoom(x, y)->getRight()) {
						opacity.a = 255;
						tile.setColor(opacity);
					}
					if (x - 1 >= 0 && maze.isSafe(maze.getRoom(x - 1, y)) && maze.getRoom(x, y)->getLeft()) {
						opacity.a = 255;
						tile.setColor(opacity);
					}
				}
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
		Color opacity = enemy.getColor();
		opacity.a = 255;
		enemy.setColor(opacity);
		if (blink <= 0) {
			if (!maze.isSafe(maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY()))) {
				opacity.a = 64;
				enemy.setColor(opacity);
				if (enemies[ptr].getY() + 1 < maze.getHeight() && maze.isSafe(maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY() + 1)) && maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->getUp()) {
					opacity.a = 255;
					enemy.setColor(opacity);
				}
				if (enemies[ptr].getY() - 1 >= 0 && maze.isSafe(maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY() - 1)) && maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->getDown()) {
					opacity.a = 255;
					enemy.setColor(opacity);
				}
				if (enemies[ptr].getX() + 1 < maze.getWidth() && maze.isSafe(maze.getRoom(enemies[ptr].getX() + 1, enemies[ptr].getY())) && maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->getRight()) {
					opacity.a = 255;
					enemy.setColor(opacity);
				}
				if (enemies[ptr].getX() - 1 >= 0 && maze.isSafe(maze.getRoom(enemies[ptr].getX() - 1, enemies[ptr].getY())) && maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->getLeft()) {
					opacity.a = 255;
					enemy.setColor(opacity);
				}
			}
			rt.draw(enemy);
		}
		ptr++;
	}

	if (pause) {
		RectangleShape pauseBackground(Vector2f(width, height));
		pauseBackground.setPosition(0, 0);
		pauseBackground.setFillColor(Color::Black);
		Color opacity = pauseBackground.getFillColor();
		opacity.a = 192;
		pauseBackground.setFillColor(opacity);
		rt.draw(pauseBackground);
		Sprite titleBar;
		Texture titleMap;
		titleMap.loadFromFile("assets/testLevelMarker.png");
		titleBar.setTexture(titleMap);
		titleBar.setScale(Vector2f(2 * scaleFactor, 2 * scaleFactor));
		titleBar.setPosition(0, 0);
		rt.draw(titleBar);
	}

	rt.display();
	window.clear();
	window.draw(Sprite(rt.getTexture()));
	window.display();
}

void update() {
	while (enemies.size() < enemyCount) {
		enemies.push_back(Enemy(enemies, maze, levels[currentLevel]["typeCount"]));
	}
	while (enemies.size() > enemyCount) {
		enemies.erase(enemies.begin() + (rand() % enemies.size()));
	}
	if (esc) {
		if (pause) { pause = false; }
		else { pause = true; }
		esc = false;
		return;
	}
	if (pause) {
		return;
	}
	collision();
	if (blink > 0) {
		blink--;
		return;
	}
	if (maze.getPlayerWorldX() == levels[currentLevel]["finishX"] && maze.getPlayerWorldY() == levels[currentLevel]["finishY"]) {
		nextLevel();
	}
	if (updateCount % mazeUpdateTime == 0 && updateCount != 0) {
		maze.regen();
	}
	if (space) {
		space = false;
		blink = blinkTime;
		maze.setFirstGen(true);
		int x;
		int y;
		for (x = 0; x < maze.getWidth(); x++) {
			for (y = 0; y < maze.getHeight(); y++) {
				maze.getRoom(x, y)->reset();
			}
		}
		maze.generateMaze();
		randomizeEnemies();
		return;
	}
	if (!moveWithPlayer && updateCount % (moveTime + 1) == 0) {
		int ptr = 0;
		while (ptr < enemies.size()) {
			maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(false);
			enemies[ptr].move(maze);
			maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(true);
			ptr++;
		}
	}
	if (Up && updateCount % (playerMoveTime + 1) == 0) {
		Up = false;
		if (maze.getRoom(maze.getPlayerX(), maze.getPlayerY())->getUp()) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				enemies[ptr].moveUp(maze);
				ptr++;
			}
		}
		maze.moveUp();
		if (moveWithPlayer) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(false);
				enemies[ptr].move(maze);
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(true);
				ptr++;
			}
		}
		return;
	}
	if (Left && updateCount % (playerMoveTime + 1) == 0) {
		Left = false;
		if (maze.getRoom(maze.getPlayerX(), maze.getPlayerY())->getLeft()) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				enemies[ptr].moveLeft(maze);
				ptr++;
			}
		}
		maze.moveLeft();
		if (moveWithPlayer) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(false);
				enemies[ptr].move(maze);
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(true);
				ptr++;
			}
		}
		return;
	}
	if (Down && updateCount % (playerMoveTime + 1) == 0) {
		Down = false;
		if (maze.getRoom(maze.getPlayerX(), maze.getPlayerY())->getDown()) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				enemies[ptr].moveDown(maze);
				ptr++;
			}
		}
		maze.moveDown();
		if (moveWithPlayer) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(false);
				enemies[ptr].move(maze);
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(true);
				ptr++;
			}
		}
		return;
	}
	if (Right && updateCount % (playerMoveTime + 1) == 0) {
		Right = false;
		if (maze.getRoom(maze.getPlayerX(), maze.getPlayerY())->getRight()) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				enemies[ptr].moveRight(maze);
				ptr++;
			}
		}
		maze.moveRight();
		if (moveWithPlayer) {
			int ptr = 0;
			while (ptr < enemies.size()) {
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(false);
				enemies[ptr].move(maze);
				maze.getRoom(enemies[ptr].getX(), enemies[ptr].getY())->setOccupied(true);
				ptr++;
			}
		}
		return;
	}
}

void randomizeEnemies() {
	while (enemies.size() > 0) {
		enemies.erase(enemies.begin());
	}
}

void nextLevel() {
	currentLevel++;
	if (currentLevel >= levels.size()) { exit("Game complete. Closing.", 0); return; }
	maze = Maze(maze.getWidth(), maze.getHeight());
	enemyCount = levels[currentLevel]["enemyCount"];
	randomizeEnemies();
}

string titleBuilder() {
	stringstream output;
	output << title;
	output << " - ";
	output << playTime();
	output << " - (";
	output << maze.getPlayerWorldX();
	output << ", ";
	output << maze.getPlayerWorldY();
	output << ")";
	return output.str();
}

void collision() {
	int ptr = 0;
	while (ptr < enemies.size()) {
		if (enemies[ptr].getX() == maze.getPlayerX() && enemies[ptr].getY() == maze.getPlayerY()) {
			maze.offsetWorldCoords((rand() % (2 * offsetMax)) - offsetMax, (rand() % (2 * offsetMax)) - offsetMax);
			space = true;
			break;
		}
		ptr++;
	}
}

string playTime() {
	stringstream output;
	long long millis = current_time.count() - start_time.count();
	int hours = (millis / (1000 * 60 * 60));
	millis %= (1000 * 60 * 60);
	int minutes = (millis / (1000 * 60));
	millis %= (1000 * 60);
	int seconds = (millis / 1000);
	millis %= 1000;
	if (hours < 10) { output << "0"; }
	output << hours;
	output << ":";
	if (minutes < 10) { output << "0"; }
	output << minutes;
	output << ":";
	if (seconds < 10) { output << "0"; }
	output << seconds;
	output << ":";
	if (millis < 100) { output << "0"; }
	if (millis < 10) { output << "0"; }
	output << millis;
	return output.str();
}