#ifndef ENDGAME_HPP
#define ENDGAME_HPP

#include <iostream>
#include <cstdint>

class EndGame3 {
public:
    uint32_t data;
    EndGame3();

    uint32_t getFromY();
    uint32_t getFromX();
    uint32_t getToY();
    uint32_t getToX();
    int32_t getMovesToMate();
    bool enable();

    void setFromY(uint32_t val);
    void setFromX(uint32_t val);
    void setToY(uint32_t val);
    void setToX(uint32_t val);
    void setMovesToMate(uint32_t moves, uint32_t color); //0 - WHITE
    void setEnable();
    
};

#endif