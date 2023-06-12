#pragma once

#include "util.h"
#include <vector>
#include <string>
#include <iostream>

struct Piece {
	PieceType kind;
	Color color;
};

#define empty_sqr Piece(open, none);

struct Board { 
private:
	Piece board[64];
	int promoting = -1;

	// graphical components
	unsigned int board_image;

public:
	Board(unsigned int); // Creates new starting board
	Board(Board*); // Copies board state
	bool makeMove(Piece, int, int);
	void promote(PieceType);
	void printBoard(std::string&);
	inline Piece getPiece(int s) { return board[s]; }
	void render(unsigned int);
};