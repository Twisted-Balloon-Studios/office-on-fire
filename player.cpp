#include "player.h"
Player::Player(): x(5), y(5), floor(0), health(100), direction(0), hasItem(false), invincibility(0){}

void Player::move(int dx, int dy){
    x += dx;
    y += dy;
}

void Player::downFloor(){
    floor++;
    x = 0; y = 0; // reset position when changing floors
}

void Player::takeDamage(int v){ // take v hit damage, can be negative
    health -= v;
    if (health < 0) health = 0;
    if (health > 100) health = 100;
}

void Player::getItem(){
    hasItem = true;
}

void Player::useItem(){
    hasItem = false;
}

int Player::get_invincibility(){ return invincibility; }

void Player::set_invincibility(int _invincibility){ invincibility = _invincibility; }