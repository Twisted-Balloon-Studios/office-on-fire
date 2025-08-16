#include <vector>
#include <utility>
#include <emscripten/bind.h>
#include "ghost.h"

using namespace emscripten;

Ghost::Ghost(int _x, int _y, int _floor, bool _active): x(_x), y(_y), floor(_floor), active(_active){
}

void Ghost::move(int _x, int _y){
    if (!active) return; // if not active, why bother
    x = _x;
    y = _y;
}

int Ghost::getX() const { return x; }
int Ghost::getY() const { return y; }
int Ghost::getFloor() const { return floor; }
bool Ghost::isActive() const { return active; }
