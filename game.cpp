#include <emscripten/bind.h>
#include <random>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
#include <cmath>
#include <queue>
#include <map>
#include "maze.h"
#include "ghost.h"
#include "player.h"
using namespace emscripten;

Player player;
Ghost ghost(12, 12, 0, true, 0);
Maze maze(20, 20, 0, 12345, player, ghost);

int calcDirection(std::pair<int,int> cur, std::pair<int,int> nxt){
    if (nxt.second > cur.second) return 2;
    if (nxt.second < cur.second) return 3;
    if (nxt.first > cur.first) return 0;
    return 1;
}

bool movePlayer(int dx, int dy){ // returns true if level incremented
    int newX = player.x + dx;
    int newY = player.y + dy;
    if (maze.getCell(newX, newY) != '#' && maze.getCell(newX, newY) != 'F'){ // 1 = wall, 10 = fire
        player.x = newX;
        player.y = newY; 
    }
    if (maze.getCell(player.x, player.y) == 'E') { // 2 = exit
        maze.generate(20, 20, player.floor + 1, player, ghost); // new floor
        // player.x = 1;
        // player.y = 1;
        // ghost.set_floor(ghost.getFloor() + 1);
        // player.floor++;
        return true;
    }
    return false;
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

    std::pair<int,int> next_step = ghost.pursue_target(closestX, closestY, maze);
    ghost.move(next_step.first, next_step.second);
    return next_step;
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
void generateMaze(int height, int width, int flr) { maze.generate(height, width, flr, player, ghost); }
void addMessage(int messageType, int x, int y, int floor, int ghost_id){
    MessageType mt = static_cast<MessageType>(messageType);
    maze.insert({mt, x, y, floor, ghost_id});
}
void cleanUp(){ maze.cleanUp(); }
void tick(){ maze.tick(); }
int tryPickup(int px, int py, int f){ 
    // return -1 if nothing to pick up, else return item ID
    int res = maze.tryPickup(px, py, f);
    if (res >= 0) player.getItem();
    return res;
}

std::pair<bool, std::pair<int,int> > tryUse(int item_code, int f){
    if (item_code == -1) return {false, {-1, -1}}; // no item, use for what
    if (item_code == 0){ // fire extinguisher
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};
        int dirx = player.x + dx[player.direction];
        int diry = player.y + dy[player.direction];
        if (maze.getCell(dirx, diry) == 'F'){
            maze.setCell(dirx, diry, '.');
            return {true, {dirx, diry}}; // extinguished the fire
        }
        return {false, {-1, -1}};
    } else if (item_code == 1){ // laser
        // illuminate(player.x, player.y);
        return {true, {player.x, player.y}};
    } else if (item_code == 2){
        return {true, {player.x, player.y}};
    } else if (item_code == 3){
        player.takeDamage(-50);
        return {true, {-1, -1}};
    }
    return {false, {-1, -1}};
}
int ghostGetDirection(){ return ghost.getDirection(); }

void illuminate(int x, int y){
    typedef std::pair<int,int> pi;
    typedef std::pair<pi, pi> pii;
    int hh = maze.getHeight();
    int ww = maze.getWidth();
    std::map<pi, pi> dist;
    std::map<pi, pi> back_pointer;
    dist[{player.x, player.y}] = {0, 0};
    std::priority_queue<pii, std::vector<pii>, std::greater<pii> > pq;
    pq.push({{0, 0}, {player.x, player.y}});
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    while (!pq.empty()){
        pii cur = pq.top();
        pq.pop();
        if (cur.second == std::make_pair(x, y)) break; // early exit
        if (dist[cur.second] < cur.first) continue; // outdated
        int curx = cur.second.first;
        int cury = cur.second.second;
        for (int i = 0; i < 4; i++){
            int newx = curx + dx[i];
            int newy = cury + dy[i];
            if (!maze.is_valid(newx, newy)) continue; // not a valid cell for inspection
            bool visited = (dist.find({newx, newy}) != dist.end());
            if (visited){
                pi prev_dist = dist[{newx, newy}];
                int blockage = maze.getCell(x, y) ? 1 : 0;
                pi new_dist = {1 + dist[cur.second].first, blockage + dist[cur.second].second};
                if (prev_dist > new_dist){
                    dist[{newx, newy}] = new_dist;
                    pq.push({dist[{newx, newy}], {newx, newy}});
                    back_pointer[{newx, newy}] = {curx, cury};
                }
            } else {
                int blockage = maze.getCell(x, y) ? 1 : 0;
                dist[{newx, newy}] = {1 + dist[cur.second].first, blockage + dist[cur.second].second};
                pq.push({dist[{newx, newy}], {newx, newy}});
                back_pointer[{newx, newy}] = {curx, cury};
            }
        }
    }

    // backtrack
    pi cur = {x, y};
    while (cur != std::make_pair(player.x, player.y)){
        if (maze.getCell(cur.first, cur.second) == '.'){
            maze.setCell(cur.first, cur.second, 'i'); // illuminate that cell
        }

        cur = back_pointer[cur];
    }
}

bool ghost_is_active(){ return ghost.isActive(); }
void ghost_toggle_active() { ghost.toggleActive(); }

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
    function("tick", &tick);
    function("illuminate", &illuminate);

    function("addMessage", &addMessage);
    function("cleanUp", &cleanUp);
    function("tryPickup", &tryPickup);
    function("tryUse", &tryUse);

    function("ghost_is_active", &ghost_is_active);
    function("ghost_toggle_active", &ghost_toggle_active);

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

