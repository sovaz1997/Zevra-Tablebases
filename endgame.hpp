#ifndef ENDGAME_HPP
#define ENDGAME_HPP

#include <cstdint>

class EndGame {
public:
    uint32_t data;
    EndGame();

    bool getColor();
    void setColor(bool color); //0 - white
};

#endif