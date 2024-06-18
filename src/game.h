#pragma once
#include "board.h"
#include "Texture.h"
#include <vector>


class Game {
private:
	Board* board = nullptr;

	Texture* queenPromotion = 0;
	Texture* rookPromotion = 0;
	Texture* knightPromotion = 0;
	Texture* bishopPromotion = 0;

public:
	Game(unsigned int);
	Game(Game*);
	~Game();

	void createPromotionTextures();
	void deletePromotionTextures();

	void render();
};