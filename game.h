#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <typeinfo>
#include "piece.h"

struct GameState {
	std::vector<std::unique_ptr<Piece>>* board;
	bool whose_turn; // 0 for white, 1 for black
};

class Game {
private:
	GameState curr_state;

public:
	void init(std::vector<std::unique_ptr<Piece>>*, bool);
	void play();
	void printBoard();
};