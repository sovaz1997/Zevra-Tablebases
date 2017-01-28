#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <ctime>
#include <string>
#include <cstdlib>
#include <random>
#include <thread>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <stack>
#include <memory.h>
#include <unistd.h> 
#include <sys/mman.h>
#include <dirent.h>
#include "bitboard.hpp"
#include "constants.hpp"
#include "killer.hpp"
#include "hash.hpp"
#include "point.hpp"
#include "timer.hpp"
#include "movearray.hpp"
#include "option.hpp"
#include "pv.hpp"
#include "extendedmove.hpp"
#include "endgame.hpp"

class TableMove {
public:
    BitMove* move;
    int mate;

	TableMove() {}
    TableMove(BitMove* _move, int _mate) : move(_move), mate(_mate) {}
    
    /*bool operator<(TableMove& right) {
        return mate < right.mate;
    }*/
};

class Game {
private:
	BitBoard game_board;
	unsigned long long nodesCounter = 0;

	const int WHITE_WIN = 100000000;
	const int BLACK_WIN = -100000000;

	int max_depth;
	bool stopped;

	int hash_decrement = 0;

	void initEngine();
	bool uciHandler(std::string str);
	void goFixedDepth();
	void goFixedTime(int time);
	void goTournament();
	void clearCash();
	double whiteUp = BLACK_WIN, blackUp = WHITE_WIN;

  std::vector<BitMove>pv;

	bool inZugzwang(BitBoard & b, uint8_t color);

	double whiteHistorySort[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	double blackHistorySort[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];

	bool hasBestMove = false;

	std::stack<BitMove>pv_line;
	int stress;
	BitMove bestMove;
	int64_t bestScore;

	//фиксированное время
	Timer timer;
	int time;
	double start_timer, end_timer;

	//турнирный режим

	double wtime, btime, winc, binc;
	int movestogo;
	bool movestogoEnable;
	uint64_t combinations;

	Option option;

public:
	Game();
	int64_t negamax(BitBoard & b, int64_t alpha, int64_t beta, double depth, int real_depth, int rule, bool inNullMove);
	int64_t negamax_elementary(BitBoard & b, int64_t alpha, int64_t beta, int depth, int real_depth, int rule, bool inNullMove);
	uint64_t perft(int depth);
	bool insufficientMaterial(std::vector<uint64_t>figureMask);
  	int64_t quies(BitBoard & b, int64_t alpha, int64_t beta, int rule, int real_depth);
	int startGame();
	void printScore(double score);
	std::string getScore(double score);
	std::vector<std::string> getStringArray(std::string str);
	bool move(std::string mv);
	void printBitBitBoard(uint64_t bit_BitBoard);
	void idPrint();

	std::vector<uint64_t>gameHash;
	std::vector<Hash>boardHash;
	uint64_t hash_cutter;

	std::vector<std::string>variant;
	std::vector<Killer>whiteKiller;
	std::vector<Killer>blackKiller;

	std::vector<MoveArray> moveArray;

	void sortAttacks(MoveArray& moves);
	void printVariant();

	//flags - begin
	bool nullMoveEnable = false;
	bool hashEnable = true;
	bool print_variant_enable = false;
	//flags - end

	double evalute_cells_size[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	void cleanWhiteHistory();
	void cleanBlackHistory();
	void flattenHistory();
	void printPV(int depth);

	double margin = PAWN_EV / 2;
	void sortMoves(MoveArray& result, int depth);
	bool recordHash(int depth, int score, int flag, uint64_t key, BitMove move, int real_depth);

	std::vector<BitMove> extractPV(int depth);

	bool testMovePossible(BitMove move);

	//tablebase functions
	void baseGenerate();

	bool setupPositionFromBase(uint64_t position, std::string mask);
	int checkMateTest();
	bool movesToMate(/*std::vector<EndGame>& positions*/EndGame* positions, std::string mask);
	uint64_t getIndex(std::string mask);

	void tableGenerate(std::string mask/*, std::vector<EndGame>& result*/);
	EndGame extractEndGame();

	int count_sym(std::string str, char c);

	std::vector<TableMove> wins;
    std::vector<TableMove> loses;
};

#endif
