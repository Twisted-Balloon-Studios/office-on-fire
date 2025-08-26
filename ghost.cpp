#include <vector>
#include <utility>
#include <emscripten/bind.h>
#include "ghost.h"

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
