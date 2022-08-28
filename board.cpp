#include "board.h"
#include "imgui.h"
#include <cctype>

const char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };

Board::Board() {
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

	// Bishop
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
}

Board::Board(Board* base) {
	for (int i = 0; i < 64; i++) board[i] = base->board[i];
}

void print_threatmap(uint64_t map) {
	std::cout << HORZ_LINE;
	int sqr;
	for (int rank = 7; rank >= 0; rank--) {
		std::cout << "| ";
		for (int file = 0; file < 8; file++) {
			sqr = BIDX(file, rank);
			std::cout << (char)(XTRC_BIT(map, sqr) + '0');
			std::cout << " | ";
		}
		std::cout << "\b\n" << HORZ_LINE;
	}
	std::cout << "\n";
}

bool Board::makeMove(Piece p, int s, int d) {
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

	// TODO: Check for promotion
	int promotion_sqr = (p.color == white) ? 7 : 0;
	if ((p.kind == pawn) && (d / 8 == promotion_sqr)) {
		promoting = d;
		return 1;
	}
	return 0;
}

void Board::promote(PieceType type) {
	if (promoting == -1) {
		std::cerr << "No piece able to promote!";
		return;
	}
	if (type == open || type == pawn) {
		std::cerr << "Invalid promotion type!";
		return;
	}

	board[promoting].kind = type;
	promoting == -1;
}

void Board::printBoard(std::string& s) {
	Piece p;
	char piece_c;
	s += HORZ_LINE;
	for (int rank = 7; rank >= 0; rank--) {
		s += "| ";
		for (int file = 0; file < 8; file++) {
			p = board[BIDX(file, rank)];
			piece_c = (p.color == white) ? tolower(piece_chars[p.kind]) : piece_chars[p.kind];
			s += piece_c;
			s += " | ";
		}
		s += "\n";
		s += HORZ_LINE;
	}
}

void Board::render() {
	std::string board_string;
	printBoard(board_string);
	ImGui::Begin("Play window", 0, ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(board_string.c_str());
	ImGui::End();
}