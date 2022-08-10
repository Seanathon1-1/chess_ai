#include "board.h"
#include <cctype>

#define ON_BOARD(v) ((0 <= v && v <= 7) ? 1 : 0)
#define BIDX(f,r) ((r)*8+f)
#define XTRC_BIT(map, bit) ((map >> bit) & 1ULL)

#define HORZ_LINE "|---|---|---|---|---|---|---|---|\n"

char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };

using namespace std;

void print_threatmap(uint64_t map);

Board::Board() {
	whose_turn = white;
	memset(&board, 0, sizeof(Piece) * 64);
	// Pawns
	for (int file = 0; file < 8; file++) {
		board[8 + file] = Piece(pawn, white);
		board[48 + file] = Piece(pawn, black);
	}

	// Rooks
	board[0] = Piece(rook, white);
	board[7] = Piece(rook, white);
	board[56] = Piece(rook, black);
	board[63] = Piece(rook, black);

	// Knights
	board[1] = Piece(knight, white);
	board[6] = Piece(knight, white);
	board[57] = Piece(knight, black);
	board[62] = Piece(knight, black);

	// Bishops
	board[2] = Piece(bishop, white);
	board[5] = Piece(bishop, white);
	board[58] = Piece(bishop, black);
	board[61] = Piece(bishop, black);

	// Queens
	board[3] = Piece(queen, white);
	board[59] = Piece(queen, black);

	// Kings
	board[4] = Piece(king, white);
	board[60] = Piece(king, black);
	white_king = 4;
	black_king = 60;
}

Board::Board(Board* base) {
	for (int i = 0; i < 64; i++) {
		board[i] = base->board[i];
	}
	white_check = base->white_check;
	black_check = base->black_check;
	white_king = base->white_king;
	black_king = base->black_king;
	whose_turn = base->whose_turn;
	white_threat_map = base->white_threat_map;
	black_threat_map = base->black_threat_map;
	white_en_passant = base->white_en_passant;
	black_en_passant = base->black_en_passant;
}

void Board::play() {
	string move;
	Piece selected;
	Piece destination;
	int sf; int sr; int df; int dr;
	int select_idx; int dest_idx;

	while (1) {
		cout << ((whose_turn == white) ? "White's " : "Black's ") << "turn:\n";
		printBoard();
		cout << "Enter move...:";
		cin >> move;

		// Moves should only be 4 characters long
		if (move.length() != 4) {
			cout << "Could not understand move, please enter a move like e2e4.\n";
			continue;
		}

		// Check that moves are in bounds
		bool skip = 0;
		for (int i = 0; i < 4; i++) {
			int val = (i % 2 == 0) ? move[i] - 'a' : move[i] - '1';
			if (!ON_BOARD(val)) {
				cout << "Move out of bounds, moves should fit within a1-h8.\n";
				cerr << "move[" << i << "]: " << val << endl;
				skip = 1;
				break;
			}
		}
		if (skip) continue;

		sf = move[0] - 'a';
		sr = move[1] - '1';
		df = move[2] - 'a';
		dr = move[3] - '1';
 		
		// check that there is a piece on the initial square
		select_idx = BIDX(sf, sr);
		selected = board[select_idx];
		if (selected.kind == open) {
			cout << "No piece selected to move.\n";
			continue;
		}

		// check that it is the correct player's turn
		if (selected.color != whose_turn) {
			cout << "Can't move other player's piece.\n";
			continue;
		}

		// check if the destination is a legal move
		dest_idx = BIDX(df, dr);
		destination = board[dest_idx];
		vector<int> moves_possible;
		legalPieceMoves(&moves_possible, selected, sf, sr);
		bool move_exists = 0;
		for (int move : moves_possible) if (move == dest_idx) { move_exists = 1; break; }
		if (!move_exists) {
			cout << "Move is not legal.\n";
			continue;
		}
		
		// Check if en passant is available for next move
		white_en_passant = -1;
		black_en_passant = -1;
		if (selected.kind == pawn && (abs(dr - sr) == 2)) (selected.color == white) ? white_en_passant = sf : black_en_passant = sf;
		if (selected.kind == king) (selected.color == white) ? white_king = BIDX(df, dr) : black_king = BIDX(df, dr);
		
		// Makes the move and changes whose turn it is
		makeMove(selected, select_idx, dest_idx);

		// Look for checkmate/stalemate
		vector<int> nextPlayerMoves;
		allLegalMoves(&nextPlayerMoves, whose_turn);
		if (nextPlayerMoves.size() == 0) {
			print_threatmap(white_threat_map);
			printBoard();
			// Checkmate
			if (whose_turn == white && white_check) {
				cout << "0-1" << endl;
				return;
			}
			else if (whose_turn == black && black_check) {
				cout << "1-0" << endl;
				return;
			}

			// Stalemate
			else {
				cout << ".5-.5" << endl;
				return;
			}
		}
	}
}

void print_threatmap(uint64_t map) {
	cout << HORZ_LINE;
	int sqr;
	for (int rank = 7; rank >= 0; rank--) {
		cout << "| ";
		for (int file = 0; file < 8; file++) {
			sqr = BIDX(file, rank);
			cout << (char)(XTRC_BIT(map, sqr) + '0');
			cout << " | ";
		}
		cout << "\b\n" << HORZ_LINE;
	}
	cout << "\n";
}

void Board::makeMove(Piece p, int s, int d) {
	// Make the move
	board[d] = p;
	board[s] = empty_sqr;

	// Extra move on castle
	if (p.kind == king && abs((s % 8) - (d % 8)) == 2) {
		if (p.color == white) {
			if (d % 8 == 6) {
				board[BIDX(5, 0)] = board[BIDX(7, 0)];
				board[BIDX(7, 0)] = empty_sqr;
			}
			if (d % 8 == 2) {
				board[BIDX(3, 0)] = board[BIDX(0, 0)];
				board[BIDX(0, 0)] = empty_sqr;
			}
		}
		if (p.color == black) {
			if (d % 8 == 6) {
				board[BIDX(5, 7)] = board[BIDX(7, 7)];
				board[BIDX(7, 7)] = empty_sqr;
			}
			if (d % 8 == 2) {
				board[BIDX(3, 7)] = board[BIDX(0, 7)];
				board[BIDX(0, 7)] = empty_sqr;
			}
		}
	}

	// Look for check
	updateThreatMaps();
	black_check = XTRC_BIT(white_threat_map, black_king);
	white_check = XTRC_BIT(black_threat_map, white_king);

	// Enforce castling restrictions
	if (p.kind == king) {
		if (p.color == white) {
			white_short_castle = 0;
			white_long_castle  = 0;
		}
		if (p.color == black) {
			black_short_castle = 0;
			black_long_castle  = 0;
		}
	}
	if (p.kind == rook) {
		if (p.color == white) {
			if (s % 8 == 7) white_short_castle = 0;
			if (s % 8 == 0) white_long_castle  = 0;
		}
		if (p.color == black) {
			if (s % 8 == 7) black_short_castle = 0;
			if (s % 8 == 0) black_long_castle  = 0;
		}
	}

	// Other player's turn
	if (whose_turn == white) whose_turn = black;
	else whose_turn = white;
}

void Board::allLegalMoves(vector<int>* moves, Color c) {
	for (int i = 0; i < 64; i++) {
		if (board[i].color == c) {
			legalPieceMoves(moves, board[i], i % 8, i / 8);
		}
	}
}

void Board::legalPieceMoves(vector<int>* moves, Piece p, int file, int rank) {
	int square; int f;

	vector<int> possible_moves;

	if (p.kind == pawn) {
		int home_rank = (p.color == white) ? 1 : 6;
		int passant_rank = (p.color == white) ? 4 : 3;
		
		// normal move
		square = BIDX(file, rank + (1 * p.color));
		if (board[square].kind == open) {
			possible_moves.push_back(square);
		}
		// pawn power
		square = BIDX(file, rank + (2 * p.color));
		if (rank == home_rank && board[square].kind == open) {
			possible_moves.push_back(square);
		}

		// captures
		pawnSights(moves, file, rank, p.color);

		// en passant
		if (rank == passant_rank) {
			f = (p.color == white) ? black_en_passant : white_en_passant;
			if (f == file - 1) possible_moves.push_back(BIDX(f, rank + (1 * p.color)));
			if (f == file + 1) possible_moves.push_back(BIDX(f, rank + (1 * p.color)));
		}
	}

	if (p.kind == knight) {
		knightSights(&possible_moves, file, rank, p.color);
	}

	if (p.kind == bishop || p.kind == queen) {
		bishopSights(&possible_moves, file, rank, p.color);
	}

	if (p.kind == rook || p.kind == queen) {
		rookSights(&possible_moves, file, rank, p.color);
	}

	if (p.kind == king) {
		kingSights(&possible_moves, file, rank, p.color);
		if (p.color == white) {
			if (white_short_castle) possible_moves.push_back(BIDX(6, 0));
			if (white_long_castle)  possible_moves.push_back(BIDX(2, 0));
		}
		if (p.color == black) {
			if (black_short_castle) possible_moves.push_back(BIDX(6, 7));
			if (black_long_castle)  possible_moves.push_back(BIDX(2, 7));
		}
	}

	// Look to see if move leaves player's king in check
	Board test_move;
	int source = BIDX(file, rank);
	for (auto iter = possible_moves.begin(); iter != possible_moves.end(); iter++) {
		test_move = Board(this);
		test_move.makeMove(p, source, *iter);
		if ((p.color == white && test_move.white_check) || (p.color == black && test_move.black_check)) continue;
		moves->push_back(*iter);
	}
}

void set_bit(uint64_t* map, uint8_t bit) {
	if (bit >= 64) {
		return;
	}
	*map |= (1ULL << bit);
}


void Board::updateThreatMaps() {
	white_threat_map = 0ULL;
	black_threat_map = 0ULL;

	Piece piece;
	vector<int> squares;
	uint64_t* threat_map;
	int f; int r;
	for (int i = 0; i < 64; i++) {
		squares.clear();
		piece = board[i];
		if (piece.kind == open) continue;
		threat_map = (piece.color == white) ? &white_threat_map : &black_threat_map;
		
		// find squares covered by this piece
		f = i % 8; r = i / 8;
		if (piece.kind == pawn) pawnSights(&squares, f, r, piece.color, true);
		if (piece.kind == knight) knightSights(&squares, f, r, piece.color, true);
		if (piece.kind == bishop || piece.kind == queen) bishopSights(&squares, f, r, piece.color, true);
		if (piece.kind == rook || piece.kind == queen) rookSights(&squares, f, r, piece.color, true);
		if (piece.kind == king) kingSights(&squares, f, r, piece.color, true);

		for (int sqr : squares) {
			//cout << "Setting the bit of square " << sqr << " from piece on square " << i << "...\n";
			set_bit(threat_map, sqr);
		}
	}
	//cout << "White Threat Map:\n";
	//print_threatmap(white_threat_map);
	//cout << "Black Threat Map:\n";
	//print_threatmap(black_threat_map);

}

void Board::pawnSights(vector<int>* moves, int file, int rank, Color c, bool threat) {
	int square;
	square = BIDX(file - 1, rank + (1 * c));
	if (file != 0 && (board[square].color == (c * -1) || threat)) moves->push_back(square);
	square = BIDX(file + 1, rank + (1 * c));
	if (file != 7 && (board[square].color == (c * -1) || threat)) moves->push_back(square);
}

void Board::knightSights(vector<int>* moves, int file, int rank, Color c, bool threat) {
	int file_moves[8] = { -2, -2, -1, -1, 1, 1, 2, 2 };
	int rank_moves[8] = { -1, 1, -2, 2, -2, 2, -1, 1 };

	int f; int r; int square;
	for (int i = 0; i < 8; i++) {
		f = file + file_moves[i];
		r = rank + rank_moves[i];
		if (ON_BOARD(f) && ON_BOARD(r)) {
			square = BIDX(f, r);
			if (board[square].color != c || threat) moves->push_back(square);
		}
	}
}

void Board::bishopSights(vector<int>* moves, int file, int rank, Color c, bool threat) {
	int f; int r; int square;

	// NW
	f = file - 1; r = rank + 1;
	while (f >= 0 && r < 8) {
		square = BIDX(f, r);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
		f--; r++;
	}

	// NE
	f = file + 1; r = rank + 1;
	while (f < 8 && r < 8) {
		square = BIDX(f, r);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
		f++; r++;
	}

	// SW
	f = file - 1; r = rank - 1;
	while (f >= 0 && r >= 0) {
		square = BIDX(f, r);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
		f--; r--;
	}

	// SE
	f = file + 1; r = rank - 1;
	while (f < 8 && r >= 0) {
		square = BIDX(f, r);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
		f++; r--;
	}
}

void Board::rookSights(vector<int>* moves, int file, int rank, Color c, bool threat) {
	int f; int r; int square;

	// left
	for (f = file - 1; f >= 0; f--) {
		square = BIDX(f, rank);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
	}

	// right
	for (f = file + 1; f < 8; f++) {
		square = BIDX(f, rank);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
	}

	// up
	for (r = rank + 1; r < 8; r++) {
		square = BIDX(file, r);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
	}

	// down
	for (r = rank - 1; r >= 0; r--) {
		square = BIDX(file, r);
		if (board[square].kind == open) moves->push_back(square);
		else if (board[square].color == c && !threat) break;
		else { moves->push_back(square); break; }
	}
}

void Board::kingSights(vector<int>* moves, int file, int rank, Color c, bool threat) {
	int file_moves[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int rank_moves[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	int f; int r; int square;
	for (int i = 0; i < 8; i++) {
		f = file + file_moves[i];
		r = rank + rank_moves[i];
		if (ON_BOARD(f) && ON_BOARD(r)) {
			square = BIDX(f, r);
			if (board[square].color != c || threat) moves->push_back(square);
		}
	}
}

void Board::printBoard() {
	Piece p;
	char piece_c;
	cout << HORZ_LINE;
	for (int rank = 7; rank >= 0; rank--) {
		cout << "| ";
		for (int file = 0; file < 8; file++) {
			p = board[BIDX(file, rank)];
			piece_c = (p.color == white) ? tolower(piece_chars[p.kind]) : piece_chars[p.kind];
			cout << piece_c;
			cout << " | ";
		}
		cout << "\b\n" << HORZ_LINE;
	}
}