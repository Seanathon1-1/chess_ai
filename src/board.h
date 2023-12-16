#pragma once

#include "util.h"
#include "shader.h"
#include <string>
#include <iostream>


class Piece;
enum Castling {	WHITE_SHORT, WHITE_LONG, BLACK_SHORT, BLACK_LONG };

class Board { 
	// essential data
	Piece* board[64];
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
	glm::ivec2 white_king;
	glm::ivec2 black_king; 
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
	Piece* held = nullptr;


	std::string printBoardString();
	void printBoardImage();
	template<class T>void promote();
	void updateThreatMaps();
	void placePiece(Piece*, int, int);
	void placePiece(Piece*, glm::ivec2);
	void clearSquare(int, int);
	void clearSquare(glm::ivec2);
public:
	Board(GLuint); // Creates new starting board
	Board(Board*); // Copies board state
	~Board();
	bool makeMove(Piece*, int, int);
	bool canCastle(Castling);
	Color whoseTurn() { return whose_turn; }
	Piece* getPiece(int f, int r) { return board[r * 8 + f]; }
	Piece* getPiece(glm::ivec2 s) { return board[s.y * 8 + s.x]; }
	bool isHolding() { return (held != nullptr); }
	int getPassantFile(Color);
	void render();
	void makeUserMove(std::string);
	void makeLegalMove(Piece*, glm::ivec2);
	bool hasLegalMove(Color);
	bool isInCheck(Color);

	void grab(Piece*);
	bool move(Piece*, glm::ivec2);
	Piece* drop();
};