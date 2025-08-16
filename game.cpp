#include <emscripten/bind.h>
#include <random>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
using namespace emscripten;

enum MessageType {
    ITEM_PICKED_UP,
    GHOST_ELIMINATED
};

struct Message {
    MessageType mt;
    int x, y, floor;
    int ghost_id;
};

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

        // add item (a fire extinguisher) at random place
        int extinguisher_x = rand() % (width-2) + 1;
        int extinguisher_y = rand() % (height-2) + 1;
        grid[extinguisher_y][extinguisher_x] = 3;
        int erasure_counter = 0;

        for (auto it = messageList.begin(); it != messageList.end(); it++){
            if (it -> floor > flr) break;
            apply(*it);
            erasure_counter += 1;
        }
        while (erasure_counter > 0){ // remove applied messages
            erasure_counter--;
            messageList.erase(messageList.begin());
        }

        // designate a 2x2 exit at bottom-right corner
        grid[height-3][width-3] = 2;
        grid[height-3][width-2] = 2;
        grid[height-2][width-3] = 2;
        grid[height-2][width-2] = 2;
    }

    bool tryPickup(int px, int py, int f){
        if (flr != f) return false; // not on the correct floor
        if (px < 0 || px >= height || py < 0 || py >= width) return false; // index out of bound
        if (grid[px][py] != 3) return false; // not item
        grid[px][py] = 0; // picked up item, empty cell now
        return true; // must send message at JS end
    }

    int getSeed() const { return seed; }
    int getHeight() const { return height; }
    int getWidth() const { return width; }
    int getCell(int x, int y) const {
        if (x < 0 || x >= height || y < 0 || y >= width) return 1; // wall
        return grid[x][y];
    }

    void apply(const Message& m){
        if (m.floor != flr) return;
        switch (m.mt){
            case ITEM_PICKED_UP:
                grid[m.x][m.y] = 0; // empty cell
                break;
            case GHOST_ELIMINATED:
                // TODO
                break;
            default:
                // TODO
                break;
        }
    }

    void apply(Message&& m){
        if (m.floor != flr) return;
        switch (m.mt){
            case ITEM_PICKED_UP:
                grid[m.x][m.y] = 0; // empty cell
                break;
            case GHOST_ELIMINATED:
                // TODO
                break;
            default:
                // TODO
                break;
        }
    }

    void insert(Message&& m){
        if (flr > m.floor) return; // not relevant
        if (flr == m.floor){ // immediately apply
            apply(std::move(m));
            return;
        }
        messageList.insert(std::move(m));
    }

    void cleanUp(){
        // cleans up the message list to only contain relevant information
        while (!messageList.empty()){
            if ((messageList.begin() -> floor) < flr){
                // not relevant information
                messageList.erase(messageList.begin());
            } else break;
        }
    }

    struct Compare {
        bool operator()(const Message& a, const Message& b) const {
            return a.floor < b.floor;
        }
    };

private:
    int height, width, seed, flr;
    std::vector<std::vector<int>> grid;
    std::set<Message, Compare> messageList;
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
void addMessage(int messageType, int x, int y, int floor, int ghost_id){
    MessageType mt = static_cast<MessageType>(messageType);
    maze.insert({mt, x, y, floor, ghost_id});
}
void cleanUp(){ maze.cleanUp(); }
bool tryPickup(int px, int py, int f){ return maze.tryPickup(px, py, f); }

EMSCRIPTEN_BINDINGS(game_module) {
    function("movePlayer", &movePlayer);
    function("getX", &getX);
    function("getY", &getY);
    function("getFloor", &getFloor);
    function("getCell", &getCell);
    function("getHeight", &getHeight);
    function("getWidth", &getWidth);
    function("generateMaze", &generateMaze);

    function("addMessage", &addMessage);
    function("cleanUp", &cleanUp);
    function("tryPickup", &tryPickup);
}
