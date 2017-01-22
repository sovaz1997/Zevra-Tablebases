#include "endgame.hpp"

EndGame3::EndGame3() : data(0) {}

uint32_t EndGame3::getFromY() {
    uint32_t mask = (1 << 31) | (1 << 30) | (1 << 29); 
    return (data & mask) >> 29;
}

uint32_t EndGame3::getFromX() {
    uint32_t mask = (1 << 28) | (1 << 27) | (1 << 26); 
    return (data & mask) >> 26;
}

uint32_t EndGame3::getToY() {
    uint32_t mask = (1 << 25) | (1 << 24) | (1 << 23); 
    return (data & mask) >> 23;
}

uint32_t EndGame3::getToX() {
    uint32_t mask = (1 << 22) | (1 << 21) | (1 << 20); 
    return (data & mask) >> 20;
}

int32_t EndGame3::getMovesToMate() {
    int32_t result = ((31 << 20) & data) >> 20;

    if(data & (1 << 14)) {
        return - result;
    }

    return result;
}

bool EndGame3::enable() {
    return (~1 & data);
}


void EndGame3::setFromY(uint32_t val) {
    uint32_t mask = ~(7 << 29);
    data = (data & mask) | (val << 29);
}

void EndGame3::setFromX(uint32_t val) {
    uint32_t mask = ~(7 << 26);
    data = (data & mask) | (val << 26);
}

void EndGame3::setToY(uint32_t val) {
    uint32_t mask = ~(7 << 23);
    data = (data & mask) | (val << 23);
}

void EndGame3::setToX(uint32_t val) {
    uint32_t mask = ~(7 << 20);
    data = (data & mask) | (val << 20);
}

void EndGame3::setMovesToMate(uint32_t moves, uint32_t color) {
    uint32_t mask = 63;
    mask <<= 19;
    data &= (~mask);

    moves <<= 15;
    color <<= 14;

    data &= (moves | color);

}

void EndGame3::setEnable() {
    if(!enable()) {
        data |= 1;
    }
}