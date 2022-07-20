// chess_ai.cpp : Defines the entry point for the application.
//

#include "chess_ai.h"

using namespace std;

void createBoard(vector<Piece*>* board) {
	// Pawns
	for (int file = 0; file < 8; file++) {
		Pawn* white_pawn = new Pawn(file, 1, 1);
		Pawn* black_pawn = new Pawn(file, 6, -1);
		board->push_back(white_pawn);
		board->push_back(black_pawn);
	}

	// Rooks
	board->push_back(new Rook(0, 0, 1));
	board->push_back(new Rook(7, 0, 1));
	board->push_back(new Rook(0, 7, -1));
	board->push_back(new Rook(7, 7, -1));

	// Knights
	board->push_back(new Knight(1, 0, 1));
	board->push_back(new Knight(6, 0, 1));
	board->push_back(new Knight(1, 7, -1));
	board->push_back(new Knight(6, 7, -1));

	// Bishops
	board->push_back(new Bishop(2, 0, 1));
	board->push_back(new Bishop(5, 0, 1));
	board->push_back(new Bishop(2, 7, -1));
	board->push_back(new Bishop(5, 7, -1));

	// Queens
	board->push_back(new Queen(3, 0, 1));
	board->push_back(new Queen(3, 7, -1));

	// Kings
	board->push_back(new King(4, 0, 1));
	board->push_back(new King(4, 7, -1));
}




int main() {
	vector<Piece*> game_board;
	createBoard(&game_board);
	
	for (auto& piece : game_board) {
		cout << typeid(*piece).name() << endl;
	}
	
	Game* game = new Game();
	game->init(&game_board, 1);
	game->printBoard();
	game->play();
	game->printBoard();
	game->play();
	game->printBoard();
	delete game;
	return 0;
}
