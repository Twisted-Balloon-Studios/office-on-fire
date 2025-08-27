#pragma once
#include <vector>
#include <set>
#include <tuple>
#include "ghost.h"
#include "player.h"

enum class MessageType {
    ITEM_PICKED_UP,
    GHOST_ELIMINATED,
    FIRE_EXTINGUISHED,
    ILLUMINATE
};

struct Message {
    MessageType mt;
    int x, y, floor;
    int ghost_id;
};

class Maze {
public:
    Maze(int h, int w, int f, int sd, Player& pl, Ghost& gh);

    void generate(int h, int w, int f, Player& pl, Ghost& gh);

    void tick();

    int tryPickup(int px, int py, int f);

    int getWidth() const;
    int getHeight() const;
    int getSeed() const;
    char getCell(int x, int y) const;
    bool is_obstacle(int x, int y) const;
    bool is_valid(int x, int y) const;
    void setCell(int x, int y, char c);

    void apply(const Message& m);
    void apply(Message&& m);

    void insert(Message&& msg);

    void cleanUp();

    void from_file(const std::string& filename, Player& pl, Ghost& gh);

    void set_p(double _p);

    struct Compare {
        bool operator()(const Message& a, const Message& b) const {
            return a.floor < b.floor;
        }
    };

private:
    int height, width, seed, flr;
    double p;
    std::vector<std::vector<char>> grid;
    std::set<Message, Compare> messageList;
    std::set<std::pair<int, int> > s;
    std::vector<char> item_codes;
};