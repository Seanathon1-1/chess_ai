#include "game.h"

using namespace std;

void Game::init(vector<unique_ptr<Piece>>* board, bool turn) {
	curr_state.board = board;
	curr_state.whose_turn = turn;
}

void Game::play() {
	// Ask for move
	cout << "Enter move...";
	string move;
	cin >> move;

	// Is move legal?
	string possible_moves = "abcdefghRNBQK";
	size_t loc = possible_moves.find(move[0]);
	if (loc != string::npos) {
		if (loc < 8) {
			 
		}
	}

	// Make move
}

void Game::printBoard() {
	char squares[64];
	memset(&squares, ' ', 64);
	for (auto& piece : *curr_state.board) {
		uint8_t piece_rank = piece->getRank();
		uint8_t piece_file = piece->getFile();
		squares[piece_rank * 8 + piece_file] = piece->getBoardRep();
		cout << typeid(*piece).name() << endl;
	}

	cerr << "|---|---|---|---|---|---|---|---|\n";
	for (int rank = 7; rank >= 0; rank--) {
		cerr << "| ";
		for (int file = 0; file < 8; file++) {
			cerr << squares[rank * 8 + file];
			cerr << " | ";
		}
		cerr << "\b\n";
		cerr << "|---|---|---|---|---|---|---|---|\n";
	}
}