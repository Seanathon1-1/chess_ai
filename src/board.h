#pragma once

#include "util.h"
#include "shader.h"
#include <string>


class Piece;
class Game;
class Board;
enum Castling {	WHITE_SHORT, WHITE_LONG, BLACK_SHORT, BLACK_LONG };


template <class T>
std::shared_ptr<T> createPiece(Color color, uint8_t position) {
	std::shared_ptr<T> newPiece = std::make_shared<T>(T(color, position));
	newPiece->createTexture();
	return newPiece;
}

class Board {
	// essential data
	std::shared_ptr<Piece> board[64];

public:
	Board();
	Board(Game*); // Creates new starting board
	Board(std::shared_ptr<Board>); // Copies board state

	void placePiece(std::shared_ptr<Piece>, int, int);
	void placePiece(std::shared_ptr<Piece>, uint8_t);
	void clearSquare(int, int);
	void clearSquare(uint8_t);

	std::shared_ptr<Piece> getPiece(int f, int r) const;
	std::shared_ptr<Piece> getPiece(uint8_t s) const;
	bool makeMove(std::shared_ptr<Piece>, uint8_t, uint8_t);
	void passantCapture(uint8_t);
	
	std::string printBoardString() const;
	
};