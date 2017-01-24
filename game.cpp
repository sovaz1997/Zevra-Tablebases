#include "game.hpp"

Game::Game() {
	wins = std::vector<TableMove>(300);
	loses = std::vector<TableMove>(300);

	initEngine();
	idPrint();
}
