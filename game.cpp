#include <emscripten/bind.h>
#include <string>
#include <cstdlib>

using namespace emscripten;

// Simple player class
class Player {
public:
    std::string id;
    int x, y;

    Player(std::string playerId) : id(playerId), x(0), y(0) {}

    void moveLeft()  { x -= 1; }
    void moveRight() { x += 1; }
    void moveUp()    { y -= 1; }
    void moveDown()  { y += 1; }

    std::string getPosition() {
        return std::to_string(x) + "," + std::to_string(y);
    }
};

// Simple floor/maze class
class Floor {
public:
    int width, height;
    Floor() {
        width = 10;
        height = 10;
    }
};

EMSCRIPTEN_BINDINGS(officeonfire) {
    class_<Player>("Player")
        .constructor<std::string>()
        .function("moveLeft", &Player::moveLeft)
        .function("moveRight", &Player::moveRight)
        .function("moveUp", &Player::moveUp)
        .function("moveDown", &Player::moveDown)
        .function("getPosition", &Player::getPosition);

    class_<Floor>("Floor")
        .constructor<>();
}