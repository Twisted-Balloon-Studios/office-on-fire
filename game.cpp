#include <emscripten/bind.h>
#include <random>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
#include <cmath>
#include "maze.h"
#include "ghost.h"
using namespace emscripten;

inline int manhattan_dist(std::pair<int,int> a, std::pair<int,int> b){
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

struct Player {
    int x, y, floor;
    Player() : x(5), y(5), floor(0) {}
    
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void downFloor() {
        floor++;
        x = 0; y = 0; // reset position when changing floors
    }
};

static Player player;
Maze maze(20, 20, 0, 12345);
static std::vector<Ghost> ghosts; // the set of ghosts that this client is responsible for

void movePlayer(int dx, int dy) {
    int newX = player.x + dx;
    int newY = player.y + dy;
    if (maze.getCell(newX, newY) != 1) { // 1 = wall
        player.x = newX;
        player.y = newY;
    }
    if (maze.getCell(player.x, player.y) == 2) { // 2 = exit
        player.floor++;
        maze.generate(20, 20, player.floor); // new floor
        player.x = 1;
        player.y = 1;
    }
}

int getX() { return player.x; }
int getY() { return player.y; }
int getFloor() { return player.floor; }
int getCell(int x, int y) { return maze.getCell(x, y); }
int getHeight() { return maze.getHeight(); }
int getWidth() { return maze.getWidth(); }
void generateMaze(int height, int width, int flr) { maze.generate(height, width, flr); }
void addMessage(int messageType, int x, int y, int floor, int ghost_id){
    MessageType mt = static_cast<MessageType>(messageType);
    maze.insert({mt, x, y, floor, ghost_id});
}
void cleanUp(){ maze.cleanUp(); }
bool tryPickup(int px, int py, int f){ return maze.tryPickup(px, py, f); }

EMSCRIPTEN_BINDINGS(game_module) {
    function("movePlayer", &movePlayer);
    function("getX", &getX);
    function("getY", &getY);
    function("getFloor", &getFloor);
    function("getCell", &getCell);
    function("getHeight", &getHeight);
    function("getWidth", &getWidth);
    function("generateMaze", &generateMaze);

    function("addMessage", &addMessage);
    function("cleanUp", &cleanUp);
    function("tryPickup", &tryPickup);
}
