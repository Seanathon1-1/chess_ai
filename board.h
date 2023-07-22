#pragma once

#include "util.h"
#include <string>
#include <iostream>

struct Piece {
	PieceType kind;
	Color color;
};

#define empty_sqr Piece(open, none);


class Square {
	bool is_black;
	glm::vec3 top_left_corner;
	glm::vec3 top_right_corner;
	glm::vec3 bottom_left_corner;
	glm::vec3 bottom_right_corner;

public:
	Square(int);
	void draw(unsigned int, bool);
	void drawTexture(std::string, unsigned int);
};


class Board { 
	// essential data
	Piece board[64];
	int promoting = -1;
 	// graphical components
	std::vector<Square*> gSquares;
	unsigned int gBoard;
	unsigned int fbo;
	unsigned int textureShaderProgram;

public:
	Board(unsigned int); // Creates new starting board
	Board(Board*); // Copies board state
	bool makeMove(Piece, int, int);
	void promote(PieceType);
	void printBoard(std::string&);
	void printBoardImage(unsigned int);
	inline Piece getPiece(int s) { return board[s]; }
	void render(unsigned int);
};