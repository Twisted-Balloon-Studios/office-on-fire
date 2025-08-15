#include <emscripten/bind.h>
#include <vector>
#include <cstdlib>

using namespace emscripten;

struct Player {
    int x;
    int y;
    int floor;
};

class Game {
public:
    std::vector<std::vector<int>> floorMaze;
    Player player;

    Game() {
        player = {0, 0, 0};
        generateFloor(player.floor);
    }

    void generateFloor(int floorNum) {
        floorMaze.clear();
        floorMaze.resize(10, std::vector<int>(10, 0));
        // Random walls
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                floorMaze[i][j] = (rand() % 4 == 0) ? 1 : 0; // 25% walls
        // Ensure player start and exit exist
        floorMaze[0][0] = 0;
        floorMaze[9][9] = 0;
    }

    void movePlayer(std::string dir) {
        int nx = player.x;
        int ny = player.y;
        if (dir == "ArrowUp") ny--;
        if (dir == "ArrowDown") ny++;
        if (dir == "ArrowLeft") nx--;
        if (dir == "ArrowRight") nx++;

        if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10 && floorMaze[ny][nx] == 0) {
            player.x = nx;
            player.y = ny;
        }

        // Check exit
        if (player.x == 9 && player.y == 9) {
            player.floor++;
            player.x = 0;
            player.y = 0;
            generateFloor(player.floor);
        }
    }

    Player getPlayer() {
        return player;
    }
};

// Bindings
EMSCRIPTEN_BINDINGS(office_on_fire) {
    class_<Player>("Player")
        .constructor<>()
        .property("x", &Player::x)
        .property("y", &Player::y)
        .property("floor", &Player::floor);

    class_<Game>("Game")
        .constructor<>()
        .function("movePlayer", &Game::movePlayer)
        .function("getPlayer", &Game::getPlayer);
}