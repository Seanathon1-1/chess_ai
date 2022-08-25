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

	// castling availability
	bool white_short_castle = 1;
	bool black_short_castle = 1;
	bool white_long_castle  = 1;
	bool black_long_castle  = 1;

	void allLegalMoves(std::vector<int>*, Color);
	void legalPieceMoves(std::vector<int>*, Piece, int, int);
	void updateThreatMaps();
	void pawnSights(std::vector<int>*, int, int, Color, bool threat = false);
	void knightSights(std::vector<int>*, int, int, Color, bool threat = false);
	void bishopSights(std::vector<int>*, int, int, Color, bool threat = false);
	void rookSights(std::vector<int>*, int, int, Color, bool threat = false);
	void kingSights(std::vector<int>*, int, int, Color, bool threat = false);

	void makeMove(Piece, int, int);
public:
	Board(); // Creates new starting board
	Board(Board*); // Copies board state
	void playMove(std::string); // game loop
	void printBoard(std::string&);
};