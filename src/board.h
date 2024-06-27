#pragma once

#include "util.h"
#include "shader.h"
#include <string>
#include <iostream>


class Piece;
class Game;
class Board;
enum Castling {	WHITE_SHORT, WHITE_LONG, BLACK_SHORT, BLACK_LONG };


template <class T>
T* createPiece(Color color, uint8_t position, Board* board) {
	T* newPiece = new T(color, position, board);
	newPiece->createTexture();
	return newPiece;
}

class Board {
	// essential data
	Piece* board[64];

public:
	Board(Game*); // Creates new starting board
	Board(Board*); // Copies board state
	~Board();
	
	void placePiece(Piece*, int, int);
	void placePiece(Piece*, uint8_t);
	void clearSquare(int, int);
	void clearSquare(uint8_t);

	Piece* getPiece(int f, int r) const { return board[r * 8 + f]; }
	Piece* getPiece(uint8_t s) const { return board[s]; }
	bool makeMove(Piece*, uint8_t, uint8_t);
	void passantCapture(uint8_t);
	
	std::string printBoardString() const;
	
};