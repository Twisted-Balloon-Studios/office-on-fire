#pragma once
#include <set>
#include "maze.h"

class Maze;

class Ghost {
public:
    Ghost(int x, int y, int f, bool active, int direction);
    void move(int x, int y); // moves to (x, y) if active
    int getX() const;
    int getY() const;
    void set_x(int _x);
    void set_y(int _y);
    void set_floor(int _floor);
    int getFloor() const;
    bool isActive() const;
    void toggleActive();
    int getDirection() const;
    std::pair<int,int> pursue_target(int tx, int ty, Maze& maze) const;

private:
    int x, y, floor, direction;
    bool active;
    std::set<std::pair<int, std::pair<int,int> > > memory;
};
