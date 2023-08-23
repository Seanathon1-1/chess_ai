#include "piece.h"

#define IN_RANGE(x, a, b) (x >= a && x <= b)

Piece::Piece(Color c, glm::vec2 square, Board* b) {
	m_color    = c;
	m_position = square;
	m_board    = b;
	m_selected = false;
}

std::vector<glm::vec2>& Knight::legalMoves(bool calculateThreats = false) {
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
		
		bool inbounds = IN_RANGE(potentialMove.x, 0, 7) && IN_RANGE(potentialMove.y, 0, 7);
		bool checkSameColor = (target->getColor() != m_color) || calculateThreats;
		if (inbounds && checkSameColor) {
			moveSquares.push_back(potentialMove);
		}

		// TODO: check for check
	}

	return moveSquares;
}

std::vector<glm::vec2>& Bishop::legalMoves(bool calculateThreats = false) {
	int f; int r; Piece* q_pieceHere;

	std::vector<glm::vec2> moveSquares;
	// We assume north to be in the positive rank direction (ie towards rank 8)
	// NW
	f = m_position.x - 1; r = m_position.y + 1;
	while (f >= 0 && r < 8) {
		q_pieceHere = m_board->getPiece(BIDX(f, r));
		if (!q_pieceHere) moveSquares.push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({f, r}); break; }
		f--; r++;
	}

	// NE
	f = m_position.x + 1; r = m_position.y + 1;
	while (f < 8 && r < 8) {
		q_pieceHere = m_board->getPiece(BIDX(f, r));
		if (!q_pieceHere) moveSquares.push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({f, r}); break; }
		f++; r++;
	}

	// SW
	f = m_position.x - 1; r = m_position.y - 1;
	while (f >= 0 && r >= 0) {
		q_pieceHere = m_board->getPiece(BIDX(f, r));
		if (!q_pieceHere) moveSquares.push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({f, r}); break; }
		f--; r--;
	}

	// SE
	f = m_position.x + 1; r = m_position.y - 1;
	while (f < 8 && r >= 0) {
		q_pieceHere = m_board->getPiece(BIDX(f, r));
		if (!q_pieceHere) moveSquares.push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({f, r}); break; }
		f++; r--;
	}

	// TODO: check for checks

	return moveSquares;
}

std::vector<glm::vec2>& Rook::legalMoves(bool calculateThreats = false) {
	int f; int r; Piece* q_pieceHere;

	std::vector<glm::vec2> moveSquares;

	// Left
	for (f = m_position.x - 1; f >= 0; f--) {
		q_pieceHere = m_board->getPiece(BIDX(f, m_position.y));
		if (!q_pieceHere) moveSquares.push_back({ f, m_position.y });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({ f, m_position }); break; }
	}

	// Right
	for (f = m_position.x + 1; f < 8; f++) {
		q_pieceHere = m_board->getPiece(BIDX(f, m_position.y));
		if (!q_pieceHere) moveSquares.push_back({ f, m_position.y });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({ f, m_position.y }); break; }
	}

	// Up
	for (r = m_position.y + 1; r < 8; r++) {
		q_pieceHere = m_board->getPiece(BIDX(m_position.x, r));
		if (!q_pieceHere) moveSquares.push_back({ m_position.x, r });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({ m_position.x, r }); break; }
	}

	// Down
	for (r = m_position.y - 1; r >= 0; r--) {
		q_pieceHere = m_board->getPiece(BIDX(m_position.x, r));
		if (!q_pieceHere) moveSquares.push_back({ m_position.x, r });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares.push_back({ m_position.x, r }); break; }
	}

	// TODO: check for checks

	return moveSquares;
}

std::vector<glm::vec2>& Queen::legalMoves(bool calculateThreats = false) {
	Bishop testBishop = Bishop(m_color, m_position, m_board);
	Rook testRook = Rook(m_color, m_position, m_board);
	
	std::vector<glm::vec2> moveSquaresBishop = testBishop.legalMoves();
	std::vector<glm::vec2> moveSquaresRook = testRook.legalMoves();

	std::vector<glm::vec2> moveSquares;
	moveSquares.reserve(moveSquaresBishop.size() + moveSquaresRook.size());
	moveSquares.insert(moveSquares.end(), moveSquaresBishop.begin(), moveSquaresBishop.end());
	moveSquares.insert(moveSquares.end(), moveSquaresRook.begin(), moveSquaresRook.end());

	return moveSquares;
}

std::vector<glm::vec2>& King::legalMoves(bool calculateThreats = false) {
	std::vector<glm::vec2> kingMoves = {
		{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
		 {1, -1},  {1, 0},  {1, 1},  {0, 1}
	};

	std::vector<glm::vec2> moveSquares;
	for (int i = 0; i < kingMoves.size(); i++) {
		glm::vec2 potentialMove = m_position + kingMoves[i];
		Piece* target = m_board->getPiece(BIDX(potentialMove.x, potentialMove.y));

		bool inbounds = IN_RANGE(potentialMove.x, 0, 7) && IN_RANGE(potentialMove.y, 0, 7);
		bool checkSameColor = (target->getColor() != m_color) || calculateThreats;
		if (inbounds && checkSameColor) {
			moveSquares.push_back(potentialMove);
		}
	}

	if (m_color == white) {
		if (white_short_castle) {
			bool transit_check = XTRC_BIT(black_threat_map, white_king + 1);
			if (!white_check && !transit_check) possible_moves.push_back(BIDX(6, 0));
		}
		if (white_long_castle) {
			bool transit_check = XTRC_BIT(black_threat_map, white_king - 1);
			if (!white_check && !transit_check) possible_moves.push_back(BIDX(2, 0));
		}
	}
	if (m_color == black) {
		if (black_short_castle) {
			bool transit_check = XTRC_BIT(white_threat_map, black_king + 1);
			if (!black_check && !transit_check) possible_moves.push_back(BIDX(6, 7));
		}
		if (black_long_castle) {
			bool transit_check = XTRC_BIT(white_threat_map, black_king - 1);
			if (!black_check && !transit_check) possible_moves.push_back(BIDX(2, 7));
		}
	}

	return moveSquares;
}

std::vector<glm::vec2>& Pawn::legalMoves(bool calculateThreats = false) {
	std::vector<glm::vec2> moveSquares;



	return moveSquares;
}