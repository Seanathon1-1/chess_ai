#pragma once
#include "game.h"

class Player {
protected:
	Game* activeGame;
	Color playerColor;
public:
	Player(Game*, Color);
	virtual void itsMyTurn() = 0;
};

class HumanPlayer : public Player {
public:
	using Player::Player;
	void itsMyTurn() override;
};

class AIPlayer : public Player {
public:
	using Player::Player;
	void itsMyTurn() override;
};