#include "endgame.hpp"

EndGame::EndGame() : data(0) {}


bool EndGame::getColor() {
    return (1 << 31) & (data);
}

void EndGame::setColor(bool color) {
    if(color) {
        data |= (1 << 31);
    } else {
        data &= ~(1 << 31);
    }
}