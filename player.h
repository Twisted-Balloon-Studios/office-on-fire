#pragma once
struct Player {
    int x, y, floor, direction;
    int health;
    bool hasItem;
    int invincibility;

    Player();
    
    void move(int dx, int dy);
    void downFloor();
    void takeDamage(int v);
    void getItem();
    void useItem();
    int get_invincibility();
    void set_invincibility(int _invincibility);
};