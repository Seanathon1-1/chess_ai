#include "board.h"
#include "piece.h"
#include "game.h"
#include "imgui.h"
#include "gl/glew.h"
#include <cctype>
#include "VAO.h"
#include "EBO.h"
#include "square.h"

const char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };



Board::Board() {
	memset(&board, 0, sizeof(Piece*) * 64);
}


/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(GLuint)
* 
* Parameters: fbo - ID of the FrameBuffer Object that the board will be drawn onto
* Description: Creates a new Board with the classical starting position
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(Game* game) {
 	memset(&board, 0, sizeof(Piece*) * 64);
	// Pawns
	for (int file = 0; file < 8; file++) {
		board[8 + file]  = createPiece<Pawn>(white, 8 + file);
		board[48 + file] = createPiece<Pawn>(black, 48 + file);
	}

	// Rooks
	board[ 0] = createPiece<Rook>(white, 0);
	board[ 7] = createPiece<Rook>(white, 7);
	board[56] = createPiece<Rook>(black, 56);
	board[63] = createPiece<Rook>(black, 63);

	// Knights
	board[ 1] = createPiece<Knight>(white, 1);
	board[ 6] = createPiece<Knight>(white, 6);
	board[57] = createPiece<Knight>(black, 57);
	board[62] = createPiece<Knight>(black, 62);

	// Bishop
	board[ 2] = createPiece<Bishop>(white, 2);
	board[ 5] = createPiece<Bishop>(white, 5);
	board[58] = createPiece<Bishop>(black, 58);
	board[61] = createPiece<Bishop>(black, 61);

	// Queens
	board[ 3] = createPiece<Queen>(white, 3);
	board[59] = createPiece<Queen>(black, 59);

	// Kings
	board[ 4] = createPiece<King>(white, 4);
	board[60] = createPiece<King>(black, 60);
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(Board*)
* 
* Parameters: base - Pointer to the board that will be copied
* Description: Makes a new board which is a copy of 'base'. Used to test moves
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(std::shared_ptr<Board> base) {
	memset(&board, 0, sizeof(Piece*) * 64);
	for (int i = 0; i < 64; i++) {
		if (base->board[i]) board[i] = (base->board[i])->copy();
		else board[i] = nullptr;
	}
}

///*-------------------------------------------------------------------------------------------------------------*\
//* print_threatmap(uint64_t)
//* 
//* Parameters: map - 64 bit representation of a threatmap
//* Description: Prints out an 8x8 ASCII representation of a threatmap to the console
//\*-------------------------------------------------------------------------------------------------------------*/
//void print_threatmap(uint64_t map) {
//	std::cout << HORZ_LINE;
//	glm::ivec2 sqr;
//	for (int rank = 7; rank >= 0; rank--) {
//		std::cout << "| ";
//		for (int file = 0; file < 8; file++) {
//			sqr = glm::ivec2( file, rank );
//			std::cout << (char)(XTRC_BIT(map, sqr.y * 8 + sqr.x) + '0');
//			std::cout << " | ";
//		}
//		std::cout << "\b\n" << HORZ_LINE;
//	}
//	std::cout << "\n";
//}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::makeMove(Piece, int, int)
* 
* Parameters: p - Piece that is being moved
*             s - Index of the source square
*             d - Index of the destination square
* Description: Handles moving a piece on the board and checks if we need to promote a pawn
* Return Value: True if we need to promote a pawn, false otherwise
\*-------------------------------------------------------------------------------------------------------------*/
bool Board::makeMove(std::shared_ptr<Piece> p, uint8_t s, uint8_t d) {
	// Make the move
	placePiece(p, d);
	clearSquare(s);

	return false;
}

void Board::passantCapture(uint8_t square) {
	clearSquare(square);
}


/*-------------------------------------------------------------------------------------------------------------*\
* Board::printBoard(std::string&)
* 
* Description: Outputs a string represention of the current state of the board
* Return: String representation of the board to be printed
\*-------------------------------------------------------------------------------------------------------------*/
std::string Board::printBoardString() const {
	std::shared_ptr<Piece> p;
	char piece_c;
	std::string s = HORZ_LINE;
	for (int rank = 7; rank >= 0; rank--) {
		s += "| ";
		for (int file = 0; file < 8; file++) {
			p = getPiece(file, rank);
			if (p) piece_c = (p->getColor() == white) ? tolower(p->textboardSymbol()) : p->textboardSymbol();
			else piece_c = ' ';
			s += piece_c;
			s += " | ";
		}
		s += "\n";
		s += HORZ_LINE;
	}
	return s;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::printBoardImage(Shader*)
* 
* Description Prints out the graphical representation of the board.
\*-------------------------------------------------------------------------------------------------------------*/


void Board::placePiece(std::shared_ptr<Piece> piece, int file, int rank) {
	placePiece(piece, rank * 8 + file );
}

void Board::placePiece(std::shared_ptr<Piece> piece, uint8_t square) {
	board[square] = piece;
	if (piece) piece->place(square);
}

void Board::clearSquare(int file, int rank) {
	placePiece(nullptr, rank * 8 + file);
}

void Board::clearSquare(uint8_t s) {
	placePiece(nullptr, s);
}

std::shared_ptr<Piece> Board::getPiece(int f, int r) const {
	return getPiece(r * 8 + f);
}
std::shared_ptr<Piece> Board::getPiece(uint8_t s) const {
	if (s < 64)  return board[s];
	else return 0;
}