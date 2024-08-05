#pragma once
#include "game.h"

class Player {
protected:
	std::shared_ptr<Game> activeGame;
	Color playerColor;
public:
	Player(std::shared_ptr<Game>, Color);
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