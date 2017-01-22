#include "game.hpp"

class TableMove {
public:
    BitMove move;
    int mate;

    TableMove(BitMove _move, int _mate) : move(_move), mate(_mate) {}
    
    bool operator<(TableMove& right) {
        return mate < right.mate;
    }
};

void Game::baseGenerate() {
    uint64_t count_positions = 2*64*64*64;
    KQk = std::vector<EndGame3>(count_positions);
    std::vector<uint8_t> legal_pos(count_positions, false);

    uint64_t counter = 0;

    for(unsigned int i = 0; i < count_positions; ++i) {
        bool legal = setupPositionFromBase(i, "KQk");
        legal_pos[i] = legal;
        counter += legal;
    }

    uint64_t tested_positions = 0;

    int count_mates = 0;
    for(unsigned int i = 0; i < count_positions; ++i) {
        if(legal_pos[i]) {
            setupPositionFromBase(i, "KQk");
            std::cout << i << " " << getIndex("KQk") << std::endl;
            if(checkMateTest()) {
                ++tested_positions;
                ++count_mates;

                KQk[i].setEnable();
                if(game_board.whiteMove) {
                    std::cout << "White in checkmate: " << game_board.getFen() << std::endl;
                    KQk[i].setMovesToMate(1, 1);
                } else {
                    std::cout << "Black in checkmate: " << game_board.getFen() << std::endl;
                    KQk[i].setMovesToMate(1, 0);
                }
            }
        }
    }

    /*for(unsigned int i = 0; i < count_positions; ++i) {
        if(legal_pos[i]) {
            setupPositionFromBase(i, "KQk");
            if(checkMateTest()) {
                ++tested_positions;
                ++count_mates;

                KQk[i].setEnable();
                if(game_board.whiteMove) {
                    std::cout << "White in checkmate: " << game_board.getFen() << std::endl;
                    KQk[i].setMovesToMate(0, 1);
                } else {
                    std::cout << "Black in checkmate: " << game_board.getFen() << std::endl;
                    KQk[i].setMovesToMate(0, 0);
                }
            }
        }
    }*/

    std::cout << "CheckMates: " << count_mates << std::endl;
}

bool Game::setupPositionFromBase(uint64_t position, std::string mask) {
    game_board.clear();
    std::vector<uint8_t> figures;

    for(unsigned int i = 0; i < mask.size(); ++i) {
        if(mask[i] == 'K') {
            figures.push_back(WHITE | KING);
        } else if(mask[i] == 'Q') {
            figures.push_back(WHITE | QUEEN);
        } else if(mask[i] == 'R') {
            figures.push_back(WHITE | ROOK);
        } else if(mask[i] == 'B') {
            figures.push_back(WHITE | BISHOP);
        } else if(mask[i] == 'N') {
            figures.push_back(WHITE | KNIGHT);
        } else if(mask[i] == 'P') {
            figures.push_back(WHITE | PAWN);
        } else if(mask[i] == 'k') {
            figures.push_back(BLACK | KING);
        } else if(mask[i] == 'q') {
            figures.push_back(BLACK | QUEEN);
        } else if(mask[i] == 'r') {
            figures.push_back(BLACK | ROOK);
        } else if(mask[i] == 'b') {
            figures.push_back(BLACK | BISHOP);
        } else if(mask[i] == 'n') {
            figures.push_back(BLACK | KNIGHT);
        } else if(mask[i] == 'p') {
            figures.push_back(BLACK | PAWN);
        }
    }

    std::vector<int> figures_positions;

    uint64_t pos_mask = 63;
    for(unsigned int i = 0; i < mask.size(); ++i) {
        figures_positions.push_back(position & pos_mask);
        position >>= 6;
    }

    if(position & 1) {
        game_board.whiteMove = false;
    } else {
        game_board.whiteMove = true;
    }

    for(unsigned int i = 0; i < mask.size(); ++i) {
        for(int j = 0; j < mask.size(); ++j) {
            if(i != j) {
                if(figures_positions[i] == figures_positions[j]) {
                    return false;
                }
            }
        }
    }

    for(unsigned int i = 0; i < figures_positions.size(); ++i) {
        game_board.fastAddFigure(figures[i], figures_positions[i] / 8, figures_positions[i] % 8);
    }

    if(game_board.inCheck(WHITE) && game_board.inCheck(BLACK)) {
        return false;
    }

    if(game_board.inCheck(WHITE) && game_board.whiteMove == false) {
        return false;
    }

    if(game_board.inCheck(BLACK) && game_board.whiteMove == true) {
        return false;
    }

    return true;
}

bool Game::checkMateTest() {
    game_board.bitBoardMoveGenerator(moveArray[0]);
    
   uint8_t color;
	if(game_board.whiteMove) {
		color = WHITE;
	} else {
		color = BLACK;
	}

    uint16_t num_of_moves = 0;

    for(unsigned int i = 0; i < moveArray[0].count; ++i) {
        game_board.move(moveArray[0].moveArray[i]);
        if(game_board.inCheck(color)) {
            game_board.goBack();
            continue;
        }

        game_board.goBack();
        ++num_of_moves;
    }

    if(!num_of_moves && game_board.inCheck(color)) {
        return true;
    }

    return false;
}

int Game::movesToMate(std::vector<EndGame3>& positions, std::string mask) {
    game_board.bitBoardMoveGenerator(moveArray[0]);
    
    uint8_t color;
    int multiple = 1;
    
	if(game_board.whiteMove) {
		color = WHITE;
	} else {
		color = BLACK;
        multiple = -1;
	}

    std::vector<TableMove> wins;
    std::vector<TableMove> loses;

    //uint16_t num_of_moves = 0;
    
    for(unsigned int i = 0; i < moveArray[0].count; ++i) {
        game_board.move(moveArray[0].moveArray[i]);
        if(game_board.inCheck(color)) {
            game_board.goBack();
            continue;
        }

        uint64_t now_index = getIndex(mask);

        if(positions[now_index].enable()) {
            if(multiple * positions[now_index].getMovesToMate() > 0) {
                wins.push_back(TableMove(moveArray[0].moveArray[i], abs(positions[now_index].getMovesToMate())));
            } else if(multiple * positions[now_index].getMovesToMate() < 0) {
                loses.push_back(TableMove(moveArray[0].moveArray[i], abs(positions[now_index].getMovesToMate())));
            }
        }

        game_board.goBack();
        //++num_of_moves;
    }

    if(wins.empty() && loses.empty()) {
        return false;
    } else if(!wins.empty()) {
        std::sort(wins.begin(), wins.end());
    }

    return true;
}

uint64_t Game::getIndex(std::string mask) { //в расчете на то, что совместна позиция и маска
    uint64_t result = 0;

    std::stack<uint8_t> factor[32];

    for(int i = 0; i < mask.size(); ++i) {
        if(mask[i] == 'K') {
            factor[WHITE | KING].push(i);
        } else if(mask[i] == 'Q') {
            factor[WHITE | QUEEN].push(i);
        } else if(mask[i] == 'R') {
            factor[WHITE | ROOK].push(i);
        } else if(mask[i] == 'B') {
            factor[WHITE | BISHOP].push(i);
        } else if(mask[i] == 'N') {
            factor[WHITE | KNIGHT].push(i);
        } else if(mask[i] == 'P') {
            factor[WHITE | PAWN].push(i);
        } else if(mask[i] == 'k') {
            factor[BLACK | KING].push(i);
        } else if(mask[i] == 'q') {
            factor[BLACK | QUEEN].push(i);
        } else if(mask[i] == 'r') {
            factor[BLACK | ROOK].push(i);
        } else if(mask[i] == 'b') {
            factor[BLACK | BISHOP].push(i);
        } else if(mask[i] == 'n') {
            factor[BLACK | KNIGHT].push(i);
        } else if(mask[i] == 'p') {
            factor[BLACK | PAWN].push(i);
        }
    }

    for(unsigned int y = 0; y < 8; ++y) {
        for(unsigned int x = 0; x < 8; ++x) {
           uint8_t figure = game_board.getFigure(y, x);

           if(figure) {
               result += (y * 8 + x) * std::pow(64, factor[figure].top());
               factor[figure].pop();
           }
        }      
    }

    if(!game_board.whiteMove) {
        return result + std::pow(2, 6 * mask.size());
    }

    return result;
}