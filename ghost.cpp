#include <vector>
#include <utility>
#include <emscripten/bind.h>
#include <queue>
#include <map>
#include "ghost.h"
#include "maze.h"

using namespace emscripten;

Ghost::Ghost(int _x, int _y, int _floor, bool _active, int _direction): x(_x), y(_y), floor(_floor), active(_active), direction(_direction){
}

void Ghost::move(int _x, int _y){
    if (!active) return; // if not active, why bother
    if (_y < y){
        direction = 3; // N
    } else if (_x > x){
        direction = 0; // E
    } else if (_y > y){
        direction = 2; // S
    } else {
        direction = 1; // W
    }
    x = _x;
    y = _y;
}

int Ghost::getX() const { return x; }
int Ghost::getY() const { return y; }
int Ghost::getFloor() const { return floor; }
bool Ghost::isActive() const { return active; }
int Ghost::getDirection() const { return direction; }
void Ghost::toggleActive() { active = !active; }
void Ghost::set_x(int _x) { x = _x; }
void Ghost::set_y(int _y) { y = _y; }
void Ghost::set_floor(int _floor) { floor = _floor; }

std::pair<int,int> Ghost::pursue_target(int tx, int ty, Maze& maze) const {
    typedef std::pair<int,int> pi;
    typedef std::pair<int, pi> pii;

    auto h = [&](int x, int y){ return abs(x - tx) + abs(y - ty); };
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    std::map<pi, int> g;
    std::map<pi, pi> backtrack;
    pi start = {x, y};
    g[start] = 0;
    pq.push({h(x, y), start});

    while (!pq.empty()){
        pi cur = pq.top().second;
        pq.pop();

        if (cur.first == tx && cur.second == ty) break;

        for (int i = 0; i < 4; i++){
            int nx = cur.first + dx[i];
            int ny = cur.second + dy[i];
            if (maze.getCell(nx, ny) == '#' || maze.getCell(nx, ny) == 'X') continue;
            int ng = g[cur] + 1;

            if (!g.count({nx, ny}) || ng < g[{nx, ny}]){
                g[{nx, ny}] = ng;
                pq.push({ng + h(nx, ny), {nx, ny}});
                backtrack[{nx, ny}] = cur;
            }
        }
    }

    pi current = {tx, ty};
    if (!backtrack.count(current)) return {x, y}; // stay in original spot
    while (backtrack[current] != start){ current = backtrack[current]; }
    return current;
}
