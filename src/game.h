#pragma once
#include "board.h"
#include "shader.h"
#include <vector>


class Game {
private:
	Board* board = nullptr;

public:
	Game(unsigned int);
	Game(Game*);
	~Game();
	void render();
};