#include "game.h"

using namespace std;

void Game::init(vector<Piece*>* board, bool turn) {
	curr_state.board = board;
	curr_state.whose_turn = turn;
}

void Game::play() {
	// Ask for move
	cout << "Enter move...";
	string move;
	cin >> move;

	// Is move legal?
	switch (move[0]) {
	case 'R':
		// Rook move
		break;
	case 'N':
		// Knight move
		break;
	case 'B':
		// Bishop move
		break;
	case 'Q':
		// Queen move
		break;
	case 'K':
		// King move
		break;
	default:
		// Pawn move or illegal
		if (move[0] >= 'a' && move[0] <= 'h') {
			// Pawn move
			uint8_t file = move[0] - 'a';
			for (auto& piece : *curr_state.board) {
				if (typeid(*piece) == typeid(Pawn) && piece->getColor() == curr_state.whose_turn && file == piece->getFile()) {
					int target_rank = stoi(move.substr(1)) - 1;
					int curr_rank = piece->getRank();
					int max_move = 1 + ((Pawn*)piece)->has_power;
					int move_dist = (target_rank - curr_rank) * piece->getColor();
					if ( move_dist > max_move || move_dist <= 0) {
						cout << "Invalid move; Try again" << endl;
						return;
					}
					piece->rank = target_rank;
				}
			}
		}
		else {
			cout << "Invalid move; Try again" << endl;
		}
	}

	// Make move
	curr_state.whose_turn *= -1;
}

void Game::printBoard() {
	char squares[64];
	memset(&squares, ' ', 64);
	for (auto& piece : *curr_state.board) {
		uint8_t piece_rank = piece->getRank();
		uint8_t piece_file = piece->getFile();
		squares[piece_rank * 8 + piece_file] = piece->getBoardRep();
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