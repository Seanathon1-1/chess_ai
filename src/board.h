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


constexpr uint8_t WHITE_SHORT_CASTLE = 0x1;
constexpr uint8_t WHITE_LONG_CASTLE = 0x2;
constexpr uint8_t BLACK_SHORT_CASTLE = 0x4;
constexpr uint8_t BLACK_LONG_CASTLE = 0x8;
constexpr uint8_t WHITE_CHECK = 0x10;
constexpr uint8_t BLACK_CHECK = 0x20;
constexpr uint8_t PROMOTING = 0x40;


class Board { 
	// essential data
	Game* game = 0;
	Piece* board[64];
	int promotionSubject = -1;
 	// graphical components
	unsigned int gBoard = 0;
	unsigned int fbo = 0;
	Shader* colorShader = nullptr;
	Shader* pieceShader = nullptr;

	uint8_t gameStatus = 0xF;
	// Where are the kings
	uint8_t white_king;
	uint8_t black_king; 
	Color whose_turn = none;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	// Which file has an en passant opportunity
	int8_t white_en_passant = -1;
	int8_t black_en_passant = -1;
	Piece* held = nullptr;


	std::string printBoardString() const;
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
	bool canCastle(Castling) const;
	Color whoseTurn() const { return whose_turn; }
	Piece* getPiece(int f, int r) const { return board[r * 8 + f]; }
	Piece* getPiece(uint8_t s) const { return board[s]; }
	bool isHolding() { return (held != nullptr); }
	bool isWaitingOnPromotion() const { return gameStatus & PROMOTING; }
	int getPassantFile(Color) const;
	void render();
	void makeUserMove(std::string);
	std::vector<uint8_t>* getLegalPieceMoves(Piece*, bool);
	void makeLegalMove(Piece*, uint8_t);
	bool hasLegalMove(Color) const;
	bool isInCheck(Color) const;
	void updateChecks();

	void grab(Piece*);
	bool move(Piece*, uint8_t);
	Piece* drop();


	template<class T>
	void promote() {
		if (promotionSubject == -1) {
			std::cerr << "No piece able to promote!";
			return;
		}

		Pawn* toDelete = (Pawn*)board[promotionSubject];
		board[promotionSubject] = createPiece<T>(toDelete->getColor(), toDelete->getPosition(), this);
		promotionSubject = -1;
		gameStatus &= ~PROMOTING;
		delete toDelete;
		game->deletePromotionTextures();

		if (whose_turn == white) whose_turn = black;
		else whose_turn = white;
	}


};