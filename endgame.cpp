#include "endgame.hpp"

EndGame::EndGame() : data(0) {}

uint32_t EndGame::getFromY() {
    uint32_t mask = (1 << 31) | (1 << 30) | (1 << 29); 
    return (data & mask) >> 29;
}

uint32_t EndGame::getFromX() {
    uint32_t mask = (1 << 28) | (1 << 27) | (1 << 26); 
    return (data & mask) >> 26;
}

uint32_t EndGame::getToY() {
    uint32_t mask = (1 << 25) | (1 << 24) | (1 << 23); 
    return (data & mask) >> 23;
}

uint32_t EndGame::getToX() {
    uint32_t mask = (1 << 22) | (1 << 21) | (1 << 20); 
    return (data & mask) >> 20;
}

int32_t EndGame::getMovesToMate() {
    /*int32_t result = ((31 << 15) & data) >> 15;

    if(data & (1 << 9)) {
        return -result;
    }*/

    int32_t result = ((2047 << 2) & data) >> 2;

    if(data & (1 << 1)) {
        return -result;
    }

    return result;
}

bool EndGame::enable() {
    return (1 & data);
}


void EndGame::setFromY(uint32_t val) {
    uint32_t mask = ~(7 << 29);
    data = (data & mask) | (val << 29);
}

void EndGame::setFromX(uint32_t val) {
    uint32_t mask = ~(7 << 26);
    data = (data & mask) | (val << 26);
}

void EndGame::setToY(uint32_t val) {
    uint32_t mask = ~(7 << 23);
    data = (data & mask) | (val << 23);
}

void EndGame::setToX(uint32_t val) {
    uint32_t mask = ~(7 << 20);
    data = (data & mask) | (val << 20);
}

void EndGame::setMovesToMate(uint32_t moves, uint32_t color) {
    /*uint32_t mask = 63;
    mask <<= 14;
    data &= (~mask);

    moves <<= 15;
    color <<= 9;

    data |= (moves | color);*/

    uint32_t mask = 4095;
    mask <<= 1;
    data &= (~mask);

    moves <<= 2;
    color <<= 1;

    data |= (moves | color);

}

void EndGame::setEnable() {
    if(!enable()) {
        data |= 1;
    }
}

void EndGame::setDisable() {
    if(enable()) {
        data &= (uint64_t)~1;
    }
}