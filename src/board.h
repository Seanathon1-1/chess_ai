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
	Game* game = 0;
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
	uint8_t white_king;
	uint8_t black_king; 
	Color whose_turn = none;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	// Which file has an en passant opportunity
	int8_t white_en_passant = -1;
	int8_t black_en_passant = -1;
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
	void updateThreatMaps();
	void placePiece(Piece*, int, int);
	void placePiece(Piece*, uint8_t);
	void clearSquare(int, int);
	void clearSquare(uint8_t);
public:
	Board(GLuint, Game*); // Creates new starting board
	Board(Board*); // Copies board state
	~Board();
	bool makeMove(Piece*, int, int);
	bool canCastle(Castling);
	Color whoseTurn() { return whose_turn; }
	Piece* getPiece(int f, int r) { return board[r * 8 + f]; }
	Piece* getPiece(uint8_t s) { return board[s]; }
	bool isHolding() { return (held != nullptr); }
	bool isWaitingOnPromotion() { return wait_for_promote; }
	int getPassantFile(Color);
	void render();
	void makeUserMove(std::string);
	std::vector<uint8_t>* getLegalPieceMoves(Piece*, bool);
	void makeLegalMove(Piece*, uint8_t);
	bool hasLegalMove(Color);
	bool isInCheck(Color);
	void updateChecks();

	void grab(Piece*);
	bool move(Piece*, uint8_t);
	Piece* drop();


	template<class T>
	void promote() {
		if (promoting == -1) {
			std::cerr << "No piece able to promote!";
			return;
		}

		Pawn* toDelete = (Pawn*)board[promoting];
		board[promoting] = createPiece<T>(toDelete->getColor(), toDelete->getPosition(), this);
		promoting = -1;
		wait_for_promote = false;
		delete toDelete;
		game->deletePromotionTextures();

		if (whose_turn == white) whose_turn = black;
		else whose_turn = white;
	}


};