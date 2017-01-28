#include "game.hpp"

void Game::baseGenerate() {
    std::string fen = game_board.getFen();
    tableGenerate("KQk");
    /*tableGenerate("kqK");
    tableGenerate("KRk");
    tableGenerate("krK");
    tableGenerate("KQkr");*/
   
    game_board.setFen(fen);
}

void Game::tableGenerate(std::string mask/*, std::vector<EndGame>& result*/) {

    FILE* file = fopen((mask + ".zev").c_str(), "ab+");
    uint64_t count_positions = 2 * pow(64, mask.size());
    
    fseek(file, 0, SEEK_END);

    if(ftell(file) < sizeof(EndGame) * count_positions) {
        EndGame* tmp_buff = new EndGame[count_positions];
        for(int i = 0; i < count_positions; ++i) {
            tmp_buff[i] = EndGame();
        }

        fwrite(tmp_buff, count_positions, sizeof(EndGame), file);
        free(tmp_buff);
    }

    fseek(file, 0, SEEK_SET);

    EndGame* result = (EndGame*)mmap(NULL, sizeof(EndGame) * count_positions, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(file), 0);

    std::vector<uint8_t> legal_pos(count_positions, false);

    uint64_t counter = 0;
    

    for(unsigned int i = 0; i < count_positions; ++i) {
        bool legal = setupPositionFromBase(i, mask);
        legal_pos[i] = legal;
        counter += legal;
    }

    uint64_t tested_positions = 0;

    int count_mates = 0;
    
    
    for(unsigned int i = 0; i < count_positions; ++i) {
        if(legal_pos[i] && !result[i].enable()) {
            setupPositionFromBase(i, mask);
            if(checkMateTest() != 0) {
                ++tested_positions;
                ++count_mates;

                result[i].setEnable();
                if(checkMateTest() < 0) {
                    std::cout << "White in checkmate: " << game_board.getFen() << std::endl;
                    result[i].setMovesToMate(1, 1);
                    
                } else if(checkMateTest() > 0) {
                    std::cout << "Black in checkmate: " << game_board.getFen() << std::endl;
                    result[i].setMovesToMate(1, 0);
                }
            }
        }
    }

    uint64_t start_timer_gen = clock();
    uint64_t old_time = start_timer_gen;
    uint64_t old_tested_positions = tested_positions;

    bool changed = true;
    while(changed) {
        changed = false;
        for(unsigned int i = 0; i < count_positions; ++i) {
            if(legal_pos[i] && !result[i].enable()) {
                setupPositionFromBase(i, mask);
                if(movesToMate(result, mask)) {
                    changed = true;
                    ++tested_positions;

                    std::cout << ((double)tested_positions / (double)counter) * 100 << "% " << ((clock() - old_time) / ((double)(tested_positions - old_tested_positions) / (double)counter) * (1 - (double)tested_positions / (double)counter)) / CLOCKS_PER_SEC << "s ";
                    
                    if(clock() - old_time >= 5 * CLOCKS_PER_SEC) {
                        old_tested_positions = tested_positions;
                        old_time = clock();
                    }

                    if(result[i].getMovesToMate() < 0) {
                        std::cout << i << "/" << count_positions << " ";
                        std::cout << "Black make checkmate in " << abs(result[i].getMovesToMate()) - 1 << ": " << game_board.getFen() << "; " << (char)(result[i].getFromX() + 'a') << (char)(result[i].getFromY() + '1') << (char)(result[i].getToX() + 'a') << (char)(result[i].getToY() + '1') << std::endl;
                    } else if(result[i].getMovesToMate() > 0) {
                        std::cout << i << "/" << count_positions << " ";
                        std::cout << "White make checkmate in " << abs(result[i].getMovesToMate()) - 1 << ": " << game_board.getFen() << "; " << (char)(result[i].getFromX() + 'a') << (char)(result[i].getFromY() + '1') << (char)(result[i].getToX() + 'a') << (char)(result[i].getToY() + '1') << std::endl;
                    }
                }
            }
        }
    }

    std::cout << "CheckMates: " << count_mates << std::endl;

    for(unsigned int i = 0; i < count_positions; ++i) {
        fwrite(&result[i], sizeof(EndGame), 1, file);
    }

    fclose(file);

    std::cout << "Success: " << mask << std::endl;
    std::cout << "Time: " << (clock() - start_timer_gen) / CLOCKS_PER_SEC << "s" << std::endl;
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
        game_board.addFigure(figures[i], figures_positions[i] / 8, figures_positions[i] % 8);
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

int Game::checkMateTest() {
    game_board.bitBoardMoveGenerator(moveArray[0]);
    
   uint8_t color;
	if(game_board.whiteMove) {
		color = WHITE;
	} else {
		color = BLACK;
	}

    uint16_t num_of_moves = 0;

    for(unsigned int i = 0; i < moveArray[0].count; ++i) {
        game_board.fastMove(moveArray[0].moveArray[i]);
        if(game_board.inCheck(color)) {
            game_board.fastGoBack();
            continue;
        }

        game_board.fastGoBack();
        ++num_of_moves;
    }

    if(!num_of_moves && game_board.inCheck(color)) {
        if(game_board.whiteMove) {
            return -1;
        } else {
            return 1;
        }
    }

    return 0;
}

bool Game::movesToMate(EndGame* positions, std::string mask) {
    game_board.bitBoardMoveGenerator(moveArray[0]);
    
    uint8_t color;
    bool col;
    int multiple = 1;
    
	if(game_board.whiteMove) {
		color = WHITE;
        col = 0;
	} else {
		color = BLACK;
        col = 1;
        multiple = -1;
	}

    int wins_count = 0, loses_count = 0;

    int num_moves = 0;
    
    for(unsigned int i = 0; i < moveArray[0].count; ++i) {
        game_board.fastMove(moveArray[0].moveArray[i]);
        
        if(game_board.inCheck(color)) {
            game_board.fastGoBack();
            continue;
        }


        uint64_t figures_count = game_board.popcount64(game_board.white_bit_mask | game_board.black_bit_mask);

        if(figures_count < mask.size()/* || !game_board.popcount64(game_board.figures[WHITE | PAWN])*/) {
            EndGame extract = extractEndGame();

            if(extract.enable()) {
                if(multiple * extract.getMovesToMate() > 0) {
                    wins[wins_count] = TableMove(&moveArray[0].moveArray[i], abs(extract.getMovesToMate()));
                    ++wins_count;
                } else if(multiple * extract.getMovesToMate() < 0) {
                    loses[loses_count] = TableMove(&moveArray[0].moveArray[i], abs(extract.getMovesToMate()));
                    ++loses_count;
                }
            }
        } else {
            uint64_t now_index = getIndex(mask);

            if(now_index == UINT64_MAX) {
                continue;
            }

            if(positions[now_index].enable()) {
                if(multiple * positions[now_index].getMovesToMate() > 0) {
                    wins[wins_count] = TableMove(&moveArray[0].moveArray[i], abs(positions[now_index].getMovesToMate()));
                    ++wins_count;
                } else if(multiple * positions[now_index].getMovesToMate() < 0) {
                    loses[loses_count] = TableMove(&moveArray[0].moveArray[i], abs(positions[now_index].getMovesToMate()));
                    ++loses_count;
                }
            }
        }

        ++num_moves;

        game_board.fastGoBack();
    }

    if(!wins_count && !loses_count) {
        return false;
    } else if(wins_count) {
        /*if(num_moves > (wins_count + loses_count) || num_moves == 0) {
            return false;
        }*/

        int index_min = 0;
        uint64_t min = UINT64_MAX;

        for(unsigned int i = 0; i < wins_count; ++i) {
            if(wins[i].mate < min) {
                min = wins[i].mate;
                index_min = i;
            }
        }

        uint64_t index = getIndex(mask);
        positions[index].setEnable();
        positions[index].setMovesToMate(abs(wins[index_min].mate) + 1, col);
        positions[index].setFromY(wins[index_min].move->fromY);
        positions[index].setFromX(wins[index_min].move->fromX);
        positions[index].setToY(wins[index_min].move->toY);
        positions[index].setToX(wins[index_min].move->toX);
    } else {
        if(num_moves > loses_count || num_moves == 0) {
            return false;
        }

        //std::sort(loses.begin(), loses.end());
        //std::reverse(loses.begin(), loses.end());

        int index_max = 0;
        uint64_t max = 0;

        for(unsigned int i = 0; i < loses_count; ++i) {
            if(loses[i].mate > max) {
                max = loses[i].mate;
                index_max = i;
            }
        }

        uint64_t index = getIndex(mask);
        positions[index].setEnable();
        positions[index].setMovesToMate(abs(loses[index_max].mate) + 1, !col);
        positions[index].setFromY(loses[index_max].move->fromY);
        positions[index].setFromX(loses[index_max].move->fromX);
        positions[index].setToY(loses[index_max].move->toY);
        positions[index].setToX(loses[index_max].move->toX);
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

    int count_figures = 0;

    uint64_t board_mask = (game_board.white_bit_mask | game_board.black_bit_mask);

    while(board_mask) {
        short k = game_board.firstOne(board_mask);
        board_mask &= ~game_board.vec1_cells[k];
        uint8_t figure = game_board.getFigure(k / 8, k % 8);

        if(!factor[figure].empty()) {
            result += k * std::pow(64, factor[figure].top());
            factor[figure].pop();
            ++count_figures;
        }
    }

    /*for(unsigned int y = 0; y < 8; ++y) {
        for(unsigned int x = 0; x < 8; ++x) {
           uint8_t figure = game_board.getFigure(y, x);

           if(figure) {
               result += (y * 8 + x) * std::pow(64, factor[figure].top());
               factor[figure].pop();
               ++count_figures;
           }
        }      
    }*/

    if(count_figures != mask.size()) {
        return UINT64_MAX;
    }

    if(!game_board.whiteMove) {
        return result + std::pow(2, 6 * mask.size());
    }

    return result;
}

EndGame Game::extractEndGame() {
    short K, Q, R, B, N, P, k, q, r, b, n, p;

    K = game_board.popcount64(game_board.figures[KING] & game_board.white_bit_mask);
    Q = game_board.popcount64(game_board.figures[QUEEN] & game_board.white_bit_mask);
    R = game_board.popcount64(game_board.figures[ROOK] & game_board.white_bit_mask);
    B = game_board.popcount64(game_board.figures[BISHOP] & game_board.white_bit_mask);
    N = game_board.popcount64(game_board.figures[KNIGHT] & game_board.white_bit_mask);
    P = game_board.popcount64(game_board.figures[PAWN] & game_board.white_bit_mask);

    k = game_board.popcount64(game_board.figures[KING] & game_board.black_bit_mask);
    q = game_board.popcount64(game_board.figures[QUEEN] & game_board.black_bit_mask);
    r = game_board.popcount64(game_board.figures[ROOK] & game_board.black_bit_mask);
    b = game_board.popcount64(game_board.figures[BISHOP] & game_board.black_bit_mask);
    n = game_board.popcount64(game_board.figures[KNIGHT] & game_board.black_bit_mask);
    p = game_board.popcount64(game_board.figures[PAWN] & game_board.black_bit_mask);

    DIR* dir = opendir(".");
    struct dirent* entry;
    std::string table;
    std::string tmp;

    bool enable = false;

    while(entry = readdir(dir)) {
        tmp = entry->d_name;

        if(count_sym(tmp, 'K') == K &&
           count_sym(tmp, 'Q') == Q &&
           count_sym(tmp, 'R') == R &&
           count_sym(tmp, 'B') == B &&
           count_sym(tmp, 'N') == N &&
           count_sym(tmp, 'P') == P &&
           count_sym(tmp, 'k') == k &&
           count_sym(tmp, 'q') == q &&
           count_sym(tmp, 'r') == r &&
           count_sym(tmp, 'b') == b &&
           count_sym(tmp, 'n') == n &&
           count_sym(tmp, 'p') == p) {

               table = tmp;
               enable = true;
               break;

        }
    }

    closedir(dir);

    if(!enable) {
        return EndGame();
    }

    FILE* file = fopen(table.c_str(), "rb");

    std::string new_mask;
    for(std::string::iterator it = table.begin(); it != table.end() && *it != '.'; ++it) {
        new_mask.push_back(*it);
    }

    uint64_t index = getIndex(new_mask);
    fseek(file, sizeof(EndGame) * index, SEEK_SET);

    EndGame result;
    fread(&result, sizeof(EndGame), 1, file);
    fclose(file);
    return result;
}

int Game::count_sym(std::string str, char c) {
    int result = 0;
    
    for(unsigned int i = 0; i < str.size(); ++i) {
        result += (str[i] == c);
    }

    return result;
}