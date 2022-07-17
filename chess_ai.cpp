// chess_ai.cpp : Defines the entry point for the application.
//

#include "chess_ai.h"

using namespace std;

void createBoard(vector<unique_ptr<Piece>>& board) {
	// Pawns
	for (int file = 0; file < 8; file++) {
		board.emplace_back(new Pawn(file, 1, 0));
		board.emplace_back(new Pawn(file, 6, 1));
	}

	// Rooks
	board.emplace_back(new Rook(0, 0, 0));
	board.emplace_back(new Rook(7, 0, 0));
	board.emplace_back(new Rook(0, 7, 1));
	board.emplace_back(new Rook(7, 7, 1));

	// Knights
	board.emplace_back(new Knight(1, 0, 0));
	board.emplace_back(new Knight(6, 0, 0));
	board.emplace_back(new Knight(1, 7, 1));
	board.emplace_back(new Knight(6, 7, 1));

	// Bishops
	board.emplace_back(new Bishop(2, 0, 0));
	board.emplace_back(new Bishop(5, 0, 0));
	board.emplace_back(new Bishop(2, 7, 1));
	board.emplace_back(new Bishop(5, 7,	1));

	// Queens
	board.emplace_back(new Queen(3, 0, 0));
	board.emplace_back(new Queen(3, 7, 1));

	// Kings
	board.emplace_back(new King(4, 0, 0));
	board.emplace_back(new King(4, 7, 1));
}




int main()
{
	vector<unique_ptr<Piece>> game_board;
	createBoard(game_board);
	Game* game = new Game();
	game->init(&game_board, 0);
	game->printBoard();
	game->play();
	delete game;
	return 0;
}
