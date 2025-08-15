#include <emscripten/bind.h>
using namespace emscripten;

struct Player {
    int x, y, floor;
    Player() : x(0), y(0), floor(0) {}
    
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void downFloor() {
        floor++;
        x = 0; y = 0; // reset position when changing floors
    }
};

Player player;

void movePlayer(int dx, int dy) {
    player.move(dx, dy);
}

int getX() { return player.x; }
int getY() { return player.y; }
int getFloor() { return player.floor; }

EMSCRIPTEN_BINDINGS(game_module) {
    function("movePlayer", &movePlayer);
    function("getX", &getX);
    function("getY", &getY);
    function("getFloor", &getFloor);
}
