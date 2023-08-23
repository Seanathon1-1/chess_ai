#include "piece.h"

#define IN_RANGE(x, a, b) (x >= a && x <= b)

Piece::Piece(Color c, bool selected, glm::vec2 square, Board* b) {
	m_color    = c;
	m_selected = selected;
	m_position = square;
	m_board    = b;
}

std::vector<glm::vec2>& Knight::legalMoves() {
	// Vectors of move directions for the knight
	std::vector<glm::vec2> knightMoves = {
		{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, 
		 {1, -2},  {1, 2},  {2, -1},  {2, 1}
	};

	// Check each potential move for legality
	std::vector<glm::vec2> moveSquares;
	for (int i = 0; i < knightMoves.size(); i++) {
		glm::vec2 potentialMove = m_position + knightMoves[i];
		Piece* target = m_board->getPiece(BIDX(potentialMove.x, potentialMove.y));
		if (IN_RANGE(potentialMove.x, 0, 7) && IN_RANGE(potentialMove.y, 0, 7) && target->getColor() != m_color) {
			moveSquares.push_back(potentialMove);
		}

		// TODO: check for check
	}

	return moveSquares;
}

std::vector<glm::vec2>& Bishop::legalMoves() {
	int f; int r; int square;

	// We assume north to be in the positive rank direction (ie towards rank 8)
	// NW
	f = m_position.x - 1; r = m_position.y + 1;
	while (f >= 0 && r < 8) {
		square = BIDX(f, r);
		if (m_board->getPiece(square)->kind == open) moves->push_back(square);
		else if (m_board->getPiece(square)->color == c && !threat) break;
		else { moves->push_back(square); break; }
		f--; r++;
	}

	// NE
	f = m_position.x + 1; r = m_position.y + 1;
	while (f < 8 && r < 8) {
		square = BIDX(f, r);
		if (m_board->getPiece(square)->kind == open) moves->push_back(square);
		else if (m_board->getPiece(square)->color == c && !threat) break;
		else { moves->push_back(square); break; }
		f++; r++;
	}

	// SW
	f = m_position.x - 1; r = m_position.y - 1;
	while (f >= 0 && r >= 0) {
		square = BIDX(f, r);
		if (m_board->getPiece(square)->kind == open) moves->push_back(square);
		else if (m_board->getPiece(square)->color == c && !threat) break;
		else { moves->push_back(square); break; }
		f--; r--;
	}

	// SE
	f = m_position.x + 1; r = m_position.y - 1;
	while (f < 8 && r >= 0) {
		square = BIDX(f, r);
		if (m_board->getPiece(square)->kind == open) moves->push_back(square);
		else if (m_board->getPiece(square)->color == c && !threat) break;
		else { moves->push_back(square); break; }
		f++; r--;
	}
}