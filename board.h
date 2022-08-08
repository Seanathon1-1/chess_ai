#pragma once

#include <vector>
#include <string>
#include <iostream>

enum PieceType { open, pawn, knight, bishop, rook, queen, king };
enum Color { black = -1, none, white };

struct Piece {
	PieceType kind;
	Color color;
};

#define empty_sqr Piece(open, none);

class Board {
private:
	Piece board[64];
	bool white_check = 0;
	bool black_check = 0;
	int white_king;
	int black_king;
	Color whose_turn = none;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	int white_en_passant = -1;
	int black_en_passant = -1;

	void legalMoves(std::vector<int>*, Piece, int, int);
	void updateThreatMaps();
	void printBoard();
	void pawnSights(std::vector<int>*, int, int, Color, bool threat = false);
	void knightSights(std::vector<int>*, int, int, Color, bool threat = false);
	void bishopSights(std::vector<int>*, int, int, Color, bool threat = false);
	void rookSights(std::vector<int>*, int, int, Color, bool threat = false);
	void kingSights(std::vector<int>*, int, int, Color, bool threat = false);

	void makeMove(Piece, int, int);
public:
	Board(); // Creates new starting board
	Board(Board*); // Copies board state
	void play(); // game loop
};