#include <emscripten/bind.h>
#include <random>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace emscripten;

class Maze {

public:
    Maze(int height, int width, int flr, int seed): height(height), width(width), seed(seed), flr(flr){
        generate(height, width, flr);
    }

    void generate(int h, int w, int f){
        height = h;
        width = w;
        flr = f;

        srand(seed*f);

        grid.assign(height, std::vector<int>(width, 0));
        for (int y = 0; y < width; y++) {
            grid[0][y] = 1;
            grid[height-1][y] = 1;
        }
        for (int x = 0; x < height; x++) {
            grid[x][0] = 1;
            grid[x][width-1] = 1;
        }

        // add random obstacles
        int numObstacles = (width * height) / 20; // 10% of grid
        for (int i = 0; i < numObstacles; i++) {
            int x = rand() % (width-2) + 1;   // avoid boundary
            int y = rand() % (height-2) + 1;
            grid[y][x] = 1;
        }

        // designate a 2x2 exit at bottom-right corner
        grid[height-3][width-3] = 2;
        grid[height-3][width-2] = 2;
        grid[height-2][width-3] = 2;
        grid[height-2][width-2] = 2;
    }

    int getSeed() const { return seed; }
    int getHeight() const { return height; }
    int getWidth() const { return width; }
    int getCell(int x, int y) const {
        if (x < 0 || x >= height || y < 0 || y >= width) return 1; // wall
        return grid[x][y];
    }

private:
    int height, width, seed, flr;
    std::vector<std::vector<int>> grid;
};

struct Player {
    int x, y, floor;
    Player() : x(5), y(5), floor(0) {}
    
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void downFloor() {
        floor++;
        x = 0; y = 0; // reset position when changing floors
    }
};

static Player player;
static Maze maze(20, 20, 0, 0);

void movePlayer(int dx, int dy) {
    int newX = player.x + dx;
    int newY = player.y + dy;
    if (maze.getCell(newX, newY) != 1) { // 1 = wall
        player.x = newX;
        player.y = newY;
    }
    if (maze.getCell(player.x, player.y) == 2) { // 2 = exit
        player.floor++;
        maze = Maze(20, 20, player.floor, maze.getSeed()); // new floor
        player.x = 1;
        player.y = 1;
    }
}

int getX() { return player.x; }
int getY() { return player.y; }
int getFloor() { return player.floor; }
int getCell(int x, int y) { return maze.getCell(x, y); }
int getHeight() { return maze.getHeight(); }
int getWidth() { return maze.getWidth(); }
void generateMaze(int height, int width, int flr) { maze.generate(height, width, flr); }

EMSCRIPTEN_BINDINGS(game_module) {
    function("movePlayer", &movePlayer);
    function("getX", &getX);
    function("getY", &getY);
    function("getFloor", &getFloor);
    function("getCell", &getCell);
    function("getHeight", &getHeight);
    function("getWidth", &getWidth);
    function("generateMaze", &generateMaze);
}
