#pragma once
struct Player {
    int x, y, floor, direction;
    int health;
    bool hasItem;

    Player();
    
    void move(int dx, int dy);
    void downFloor();
    void takeDamage(int v);
    void getItem();
    void useItem();
};