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

struct Player {
    int x, y, floor, direction;
    int health;
    bool hasItem;
    Player() : x(5), y(5), floor(0), health(100), direction(0), hasItem(false) {}
    
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void downFloor() {
        floor++;
        x = 0; y = 0; // reset position when changing floors
    }

    void takeDamage(int v){ // take v hit damage, can be negative
        health -= v;
        if (health < 0) health = 0;
        if (health > 100) health = 100;
    }

    void getItem(){
        hasItem = true;
    }

    void useItem(){
        hasItem = false;
    }
};

static Player player;
Maze maze(20, 20, 0, 12345);
Ghost ghost(12, 12, 0, true, 0);

int calcDirection(std::pair<int,int> cur, std::pair<int,int> nxt){
    if (nxt.second > cur.second) return 2;
    if (nxt.second < cur.second) return 3;
    if (nxt.first > cur.first) return 0;
    return 1;
}

void movePlayer(int dx, int dy) {
    int newX = player.x + dx;
    int newY = player.y + dy;
    if (maze.getCell(newX, newY) != '#' && maze.getCell(newX, newY) != 'F'){ // 1 = wall, 10 = fire
        player.x = newX;
        player.y = newY; 
    }
    if (maze.getCell(player.x, player.y) == 'E') { // 2 = exit
        player.floor++;
        maze.generate(20, 20, player.floor); // new floor
        player.x = 1;
        player.y = 1;
    }
}

inline int manhattan_dist(std::pair<int,int> a, std::pair<int,int> b){
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

// logic for ghost movement
std::pair<int,int> moveGhost(const std::vector<std::pair<int,int>>& playerCoords) {
    int ghostX = ghost.getX();
    int ghostY = ghost.getY();

    int closestX = ghostX, closestY = ghostY;
    int minDist = INT_MAX;

    // find nearest player
    for (auto [px, py] : playerCoords) {
        int dist = manhattan_dist({px, py}, {ghostX, ghostY});
        if (dist < minDist) {
            minDist = dist;
            closestX = px;
            closestY = py;
        }
    }

    // compute tentative next step
    int nextX = ghostX;
    int nextY = ghostY;

    if (closestX > ghostX) nextX++;
    else if (closestX < ghostX) nextX--;
    if (closestY > ghostY) nextY++;
    else if (closestY < ghostY) nextY--;

    if (maze.getCell(nextX, nextY) != '#' && maze.getCell(nextX, nextY) != 'F'){ // impending cell is not a wall nor fire
        ghostX = nextX;
        ghostY = nextY;
    }

    ghost.move(ghostX, ghostY); // persist information in ghost

    return {ghostX, ghostY};
}

int getX(){ return player.x; }
int getY(){ return player.y; }
int getFloor(){ return player.floor; }
int getHealth(){ return player.health; }
int getDirection(){ return player.direction; }
void setDirection(int dir){ player.direction = dir; }
void takeDamage(int v){ player.takeDamage(v); }
std::string getCell(int x, int y){ 
    if (ghost.getX() == x && ghost.getY() == y && ghost.isActive()) return "G"; // ghost there
    return std::string(1, maze.getCell(x, y)); 
}
int getHeight(){ return maze.getHeight(); }
int getWidth(){ return maze.getWidth(); }
void generateMaze(int height, int width, int flr) { maze.generate(height, width, flr); }
void addMessage(int messageType, int x, int y, int floor, int ghost_id){
    MessageType mt = static_cast<MessageType>(messageType);
    maze.insert({mt, x, y, floor, ghost_id});
}
void cleanUp(){ maze.cleanUp(); }
bool tryPickup(int px, int py, int f){ 
    bool res = maze.tryPickup(px, py, f);
    if (res) player.getItem();
    return res;
}

std::pair<bool, std::pair<int,int> > tryUse(int f){
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    int dirx = player.x + dx[player.direction];
    int diry = player.y + dy[player.direction];
    if (maze.getCell(dirx, diry) == 'F'){
        maze.setCell(dirx, diry, '.');
        return {true, {dirx, diry}}; // extinguished the fire
    }
    return {false, {-1, -1}};
}
int ghostGetDirection(){ return ghost.getDirection(); }

EMSCRIPTEN_BINDINGS(game_module) {
    function("movePlayer", &movePlayer);
    function("getX", &getX);
    function("getY", &getY);
    function("getHealth", &getHealth);
    function("takeDamage", &takeDamage);
    function("getFloor", &getFloor);
    function("getCell", &getCell);
    function("getHeight", &getHeight);
    function("getDirection", &getDirection);
    function("setDirection", &setDirection);
    function("getWidth", &getWidth);
    function("generateMaze", &generateMaze);

    function("addMessage", &addMessage);
    function("cleanUp", &cleanUp);
    function("tryPickup", &tryPickup);
    function("tryUse", &tryUse);

    function("moveGhost", &moveGhost);
    function("ghostGetDirection", &ghostGetDirection);
    register_vector<std::pair<int,int>>("VectorPairIntInt"); // to bind vector<pair<int, int> >
    value_array<std::pair<int,int>>("PairIntInt") // to bind pair<int,int> 
        .element(&std::pair<int,int>::first)
        .element(&std::pair<int,int>::second);
    value_array<std::pair<bool, std::pair<int,int> > >("BoolPairIntInt")
        .element(&std::pair<bool, std::pair<int,int> >::first)
        .element(&std::pair<bool, std::pair<int,int> >::second);
}

