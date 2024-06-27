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
		board[8 + file]  = createPiece<Pawn>(white, 8 + file, this);
		board[48 + file] = createPiece<Pawn>(black, 48 + file, this);
	}

	// Rooks
	board[ 0] = createPiece<Rook>(white, 0, this);
	board[ 7] = createPiece<Rook>(white, 7, this);
	board[56] = createPiece<Rook>(black, 56, this);
	board[63] = createPiece<Rook>(black, 63, this);

	// Knights
	board[ 1] = createPiece<Knight>(white, 1, this);
	board[ 6] = createPiece<Knight>(white, 6, this);
	board[57] = createPiece<Knight>(black, 57, this);
	board[62] = createPiece<Knight>(black, 62, this);

	// Bishop
	board[ 2] = createPiece<Bishop>(white, 2, this);
	board[ 5] = createPiece<Bishop>(white, 5, this);
	board[58] = createPiece<Bishop>(black, 58, this);
	board[61] = createPiece<Bishop>(black, 61, this);

	// Queens
	board[ 3] = createPiece<Queen>(white, 3, this);
	board[59] = createPiece<Queen>(black, 59, this);

	// Kings
	board[ 4] = createPiece<King>(white, 4, this);
	board[60] = createPiece<King>(black, 60, this);
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(Board*)
* 
* Parameters: base - Pointer to the board that will be copied
* Description: Makes a new board which is a copy of 'base'. Used to test moves
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(Board* base) {
	for (int i = 0; i < 64; i++) {
		if (base->board[i]) board[i] = (base->board[i])->copy(this);
		else board[i] = nullptr;
	}
}

// Performs cleanup before deleting the board
Board::~Board() {
	for (Piece* p : board) if (p) delete p;
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
bool Board::makeMove(Piece* p, uint8_t s, uint8_t d) {
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
	Piece* p;
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


void Board::placePiece(Piece* piece, int file, int rank) {
	placePiece(piece, rank * 8 + file );
}

void Board::placePiece(Piece* piece, uint8_t square) {
	board[square] = piece;
	if (piece) piece->place(square);
}

void Board::clearSquare(int file, int rank) {
	placePiece(nullptr, rank * 8 + file);
}

void Board::clearSquare(uint8_t s) {
	placePiece(nullptr, s);
}