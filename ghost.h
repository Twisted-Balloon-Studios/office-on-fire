#pragma once
#include <vector>

class Ghost {
public:
    Ghost(int x, int y, int f, bool active);
    void move(int x, int y); // moves to (x, y) if active
    int getX() const;
    int getY() const;
    int getFloor() const;
    bool isActive() const;

private:
    int x, y, floor;
    bool active;
};
