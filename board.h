#pragma once

#include "util.h"
#include "shader.h"
#include "piece.h"
#include <string>
#include <iostream>


struct Square {
	glm::vec3 top_left_corner;
	bool is_dark;
	Piece* piece;

public:
	Square(glm::vec3 tlc, bool dark, Piece* p) : top_left_corner(tlc), is_dark(dark), piece(p) {}
	void draw(Shader*);
	void drawTexture(Shader*);
};


class Board { 
	// essential data
	Piece* board[64];
	int promoting = -1;
 	// graphical components
	unsigned int gBoard = 0;
	unsigned int fbo = 0;
	Shader* colorShader = nullptr;
	Shader* pieceShader = nullptr;

public:
	Board(GLuint); // Creates new starting board
	Board(Board*); // Copies board state
	~Board();
	bool makeMove(Piece*, int, int);
	void promote(PieceType);
	std::string printBoardString();
	void printBoardImage();
	inline Piece* getPiece(int s) { return &board[s]; }
	void render();
};