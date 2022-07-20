#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <typeinfo>
#include <string>
#include "piece.h"

struct GameState {
	std::vector<Piece*>* board;
	int whose_turn; // 1 for white, -1 for black
};

class Game {
private:
	GameState curr_state;

public:
	void init(std::vector<Piece*>*, bool);
	void play();
	void printBoard();
};