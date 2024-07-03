#pragma once
#include "game.h"

class Player {
protected:
	Game* activeGame;
	Color playerColor;
public:
	Player(Game*, Color);
};

class HumanPlayer : public Player {
public:
	using Player::Player;
};

class AIPlayer : public Player {
public:
	using Player::Player;
	void pickLegalMove();
};