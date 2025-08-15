#include <emscripten/bind.h>
#include <string>
using namespace emscripten;

class Player {
public:
    int x = 0;
    int y = 0;

    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    std::string position() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// expose the Player class to JS
EMSCRIPTEN_BINDINGS(officeonfire_module) {
    class_<Player>("Player")
        .constructor<>()
        .function("move", &Player::move)
        .function("position", &Player::position);
}
