#pragma once

#include "util.h"
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

struct Board { 
private:
	Piece board[64];

public:
	Board(); // Creates new starting board
	Board(Board*); // Copies board state
	bool makeMove(Piece, int, int);
	void printBoard(std::string&);
	inline Piece getPiece(int s) { return board[s]; }
	void render();
};