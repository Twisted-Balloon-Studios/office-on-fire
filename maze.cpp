#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "maze.h"

Maze::Maze(int h, int w, int f, int sd): height(h), width(w), seed(sd), flr(f){
    generate(h, w, f);
}

void Maze::generate(int h, int w, int f){
    if (f <= 1){
        height = h;
        width = w;
        flr = f;
        srand(seed*f);
        from_file("floors/floor" + std::to_string(f) + ".txt");
    } else {
        height = h;
        width = w;
        flr = f;

        srand(seed*f);

        grid.assign(height, std::vector<char>(width, '.'));
        for (int y = 0; y < width; y++) {
            grid[0][y] = '#';
            grid[height-1][y] = '#';
        }
        for (int x = 0; x < height; x++) {
            grid[x][0] = '#';
            grid[x][width-1] = '#';
        }

        // add random obstacles
        int numObstacles = (width * height) / 20; // 10% of grid
        for (int i = 0; i < numObstacles; i++) {
            int x = rand() % (width-2) + 1;   // avoid boundary
            int y = rand() % (height-2) + 1;
            grid[y][x] = 'F'; // fire
        }

        // add item (a fire extinguisher) at random place
        int extinguisher_x = rand() % (width-2) + 1;
        int extinguisher_y = rand() % (height-2) + 1;
        grid[extinguisher_y][extinguisher_x] = 'I';
        // designate a 2x2 exit at bottom-right corner
        grid[height-3][width-3] = 'E';
        grid[height-3][width-2] = 'E';
        grid[height-2][width-3] = 'E';
        grid[height-2][width-2] = 'E';
    }

    int erasure_counter = 0;

    for (auto it = messageList.begin(); it != messageList.end(); it++){
        if (it -> floor > flr) break;
        if (it -> floor < flr){
            erasure_counter += 1;
            continue;
        }
        apply(*it);
        erasure_counter += 1;
    }
    while (erasure_counter > 0){ // remove applied messages
        erasure_counter--;
        messageList.erase(messageList.begin());
    }
}

bool Maze::tryPickup(int px, int py, int f){
    if (flr != f) return false; // not on the correct floor
    if (px < 0 || px >= height || py < 0 || py >= width) return false; // index out of bound
    if (grid[px][py] != 'I') return false; // not item
    grid[px][py] = '.'; // picked up item, empty cell now
    return true; // must send message at JS end
}

int Maze::getSeed() const { return seed; }
int Maze::getHeight() const { return height; }
int Maze::getWidth() const { return width; }
char Maze::getCell(int x, int y) const {
    if (x < 0 || x >= height || y < 0 || y >= width) return '#'; // wall
    return grid[x][y];
}
void Maze::setCell(int x, int y, char c){
    if (x < 0 || x >= height || y < 0 || y >= width) return;
    grid[x][y] = c;
}

void Maze::apply(const Message& m){
    if (m.floor != flr) return;
    switch (m.mt){
        case MessageType::ITEM_PICKED_UP:
            grid[m.x][m.y] = '.'; // empty cell
            break;
        case MessageType::GHOST_ELIMINATED:
            // TODO
            break;
        case MessageType::FIRE_EXTINGUISHED:
            grid[m.x][m.y] = '.'; // empty cell
            break;
        default:
            // TODO
            break;
    }
}

void Maze::apply(Message&& m){
    if (m.floor != flr) return;
    switch (m.mt){
        case MessageType::ITEM_PICKED_UP:
            grid[m.x][m.y] = '.'; // empty cell
            break;
        case MessageType::GHOST_ELIMINATED:
            // TODO
            break;
        case MessageType::FIRE_EXTINGUISHED:
            grid[m.x][m.y] = '.'; // empty cell
            break;
        default:
            // TODO
            break;
    }
}

void Maze::insert(Message&& m){
    if (flr > m.floor) return; // not relevant
    if (flr == m.floor){ // immediately apply
        apply(std::move(m));
        return;
    }
    messageList.insert(std::move(m));
}

void Maze::cleanUp(){
    // cleans up the message list to only contain relevant information
    while (!messageList.empty()){
        if ((messageList.begin() -> floor) < flr){
            // not relevant information
            messageList.erase(messageList.begin());
        } else break;
    }
}

void Maze::from_file(const std::string& filename){
    std::ifstream infile(filename);
    if (!infile.is_open()) throw std::runtime_error("Could not open maze file: " + filename);

    std::vector<std::vector<char>> newGrid;
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty()) continue; // skip empty lines

        std::vector<char> row;
        for (char c : line) {
            if (c == ' ' || c == '\t') continue; // skip whitespace
            row.push_back(c);
        }
        if (!row.empty()) {
            newGrid.push_back(row);
        }
    }

    infile.close();

    if (newGrid.empty()) throw std::runtime_error("Maze file is empty or invalid: " + filename);

    height = static_cast<int>(newGrid.size());
    width = static_cast<int>(newGrid[0].size());

    for (const auto& row : newGrid) {
        if (static_cast<int>(row.size()) != width) throw std::runtime_error("Maze file has inconsistent row widths.");
    }

    grid = std::move(newGrid);
}

