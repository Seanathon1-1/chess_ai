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

private:
	void makeUserMove(std::string);
	void makeLegalMove(Piece*, int, int);
	void allLegalMoves(std::vector<int>*, Color);
	void legalPieceMoves(std::vector<int>*, Piece*, int, int);
	void pawnSights(std::vector<int>*, int, int, Color, bool threat = false);
	void knightSights(std::vector<int>*, int, int, Color, bool threat = false);
	void bishopSights(std::vector<int>*, int, int, Color, bool threat = false);
	void rookSights(std::vector<int>*, int, int, Color, bool threat = false);
	void kingSights(std::vector<int>*, int, int, Color, bool threat = false);
	void updateThreatMaps();
};