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
	Piece* selected = nullptr;
	int promoting = -1;
 	// graphical components
	unsigned int gBoard = 0;
	unsigned int fbo = 0;
	Shader* colorShader = nullptr;
	Shader* pieceShader = nullptr;

	// Who's in check
	bool white_check = 0;
	bool black_check = 0;
	// Where are the kings
	int white_king;
	int black_king;
	Color whose_turn = none;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	// Which file has an en passant opportunity
	int white_en_passant = -1;
	int black_en_passant = -1;
	// Castling availabilities
	bool white_short_castle = 1;
	bool black_short_castle = 1;
	bool white_long_castle = 1;
	bool black_long_castle = 1;
	// Are we waiting for the player to decide how to promote their pawn
	bool wait_for_promote = 0;
	friend class Piece;


public:
	Board(GLuint); // Creates new starting board
	Board(Board*); // Copies board state
	~Board();
	bool makeMove(Piece*, int, int);
	template<class T>
	void promote();
	std::string printBoardString();
	void printBoardImage();
	Piece* getPiece(int s) { return &board[s]; }
	Color whoseTurn() { return whose_turn; }
	void render();
};