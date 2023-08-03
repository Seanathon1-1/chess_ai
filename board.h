#pragma once

#include "util.h"
#include "shader.h"
#include <string>
#include <iostream>

struct Piece {
	PieceType kind;
	Color color;
};

#define empty_sqr Piece(open, none);


struct Square {
	glm::vec3 top_left_corner;
	bool is_black;
	Piece piece;
	void draw(Shader, unsigned int, bool);
};


struct Board { 
private:
	// essential data
	Piece board[64];
	int promoting = -1;
 	// graphical components
	std::vector<Square*> gSquares;
	Shader defaultShader;
	unsigned int gBoard;
	unsigned int fbo;

public:
	Board(unsigned int); // Creates new starting board
	Board(Board*); // Copies board state
	bool makeMove(Piece, int, int);
	void promote(PieceType);
	void printBoard(std::string&);
	void printBoardImage();
	inline Piece getPiece(int s) { return board[s]; }
	void render();
};