#include "piece.h"

#define IN_RANGE(x, a, b) (x >= a && x <= b)

Piece::Piece(Color c, uint8_t square, Board* b) {
	m_color    = c;
	m_position = square;
	m_board    = b;
	m_selected = false;
	m_texture  = 0;
}

Piece::~Piece() {
	if (m_texture) delete m_texture;
}

void Piece::createTexture() {
	m_texture = new Texture(this);
}

Piece* Knight::copy(Board* newBoard) {
	return new Knight(m_color, m_position, newBoard);
}

void Knight::possibleMoves(std::vector<Move>* moves, bool calculateThreats = false) {
	// Vectors of move directions for the knight
	std::vector<int8_t> knightMoves = { -17, -15, -10, -6, 6, 10, 15, 17 };

	// Check each potential move for legality
	for (int i = 0; i < knightMoves.size(); i++) {
		int8_t moveRankOffset = (uint8_t)lround(knightMoves[i] / 8.f);
		int8_t moveFileOffset = knightMoves[i] - moveRankOffset * 8;
		int8_t potentialMoveFile = m_position % 8 + moveFileOffset;
		int8_t potentialMoveRank = m_position / 8 + moveRankOffset;

		if (!ON_BOARD(potentialMoveFile) || !ON_BOARD(potentialMoveRank)) continue;
		Move potentialMove = { this, m_position,(uint8_t)(m_position + knightMoves[i]) };
		Piece* targetPiece = m_board->getPiece(potentialMove.target);
		Color target_color = (targetPiece) ? targetPiece->getColor() : none;

		bool checkSameColor = (target_color != m_color) || calculateThreats;
		if (checkSameColor) {
			moves->push_back(potentialMove);
		}
	}
}

Piece* Bishop::copy(Board* newBoard) {
	return new Bishop(m_color, m_position, newBoard);
}

void Bishop::possibleMoves(std::vector<Move>* moves, bool calculateThreats = false) {
	int8_t f; int8_t r; Piece* q_pieceHere;

	uint8_t currentFile = m_position % 8;
	uint8_t currentRank = m_position / 8;
	uint8_t moveSquare;

	// We assume north to be in the positive rank direction (ie towards rank 8)
	// NW
	f = currentFile - 1; r = currentRank + 1;
	while (f >= 0 && r < 8) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({ this, m_position, moveSquare });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({ this, m_position, moveSquare }); break; }
		f--; r++;
	}

	// NE
	f = currentFile + 1; r = currentRank + 1;
	while (f < 8 && r < 8) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({ this, m_position, moveSquare });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({this, m_position, moveSquare}); break; }
		f++; r++;
	}

	// SW
	f = currentFile - 1; r = currentRank - 1;
	while (f >= 0 && r >= 0) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({this, m_position, moveSquare});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({this, m_position, moveSquare}); break; }
		f--; r--;
	}

	// SE
	f = currentFile + 1; r = currentRank - 1;
	while (f < 8 && r >= 0) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({this, m_position, moveSquare});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({this, m_position, moveSquare}); break; }
		f++; r--;
	}
}

Piece* Rook::copy(Board* newBoard) {
	return new Rook(m_color, m_position, newBoard);
}

void Rook::possibleMoves(std::vector<Move>* moves, bool calculateThreats = false) {
	int f; int r; Piece* q_pieceHere;

	uint8_t currentFile = m_position % 8;
	uint8_t currentRank = m_position / 8;
	uint8_t moveSquare;

	// Left
	for (f = currentFile - 1; f >= 0; f--) {
		moveSquare = currentRank * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({this, m_position, moveSquare});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({this, m_position, moveSquare}); break; }
	}

	// Right
	for (f = currentFile + 1; f < 8; f++) {
		moveSquare = currentRank * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({this, m_position, moveSquare});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({this, m_position, moveSquare}); break; }
	}

	// Up
	for (r = currentRank + 1; r < 8; r++) {
		moveSquare = r * 8 + currentFile;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({this, m_position, moveSquare});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({this, m_position, moveSquare}); break; }
	}

	// Down
	for (r = currentRank - 1; r >= 0; r--) {
		moveSquare = r * 8 + currentFile;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moves->push_back({this, m_position, moveSquare});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moves->push_back({ this, m_position, moveSquare }); break; }
	}
}

Piece* Queen::copy(Board* newBoard) {
	return new Queen(m_color, m_position, newBoard);
}

void Queen::possibleMoves(std::vector<Move>* moves, bool calculateThreats = false) {
	std::vector<Move> candidates;
	
	Bishop testBishop = Bishop(m_color, m_position, m_board);
	Rook testRook = Rook(m_color, m_position, m_board);
	
	testBishop.possibleMoves(&candidates);
	testRook.possibleMoves(&candidates);

	for (Move move : candidates) {
		moves->push_back({ this, move.source, move.target });
	}
}

Piece* King::copy(Board* newBoard) {
	return new King(m_color, m_position, newBoard);
}

void King::possibleMoves(std::vector<Move>* moves, bool calculateThreats = false) {
	std::vector<int8_t> kingMoves = { -9, -8, -7, -1, 1, 7, 8, 9 };

	for (int i = 0; i < kingMoves.size(); i++) {
		int8_t moveRankOffset = (uint8_t)lround(kingMoves[i] / 8.f);
		int8_t moveFileOffset = kingMoves[i] - moveRankOffset * 8;
		int8_t potentialMoveFile = m_position % 8 + moveFileOffset;
		int8_t potentialMoveRank = m_position / 8 + moveRankOffset;

		if (!ON_BOARD(potentialMoveFile) || !ON_BOARD(potentialMoveRank)) continue;
		uint8_t potentialMove = m_position + kingMoves[i];
		Piece* target = m_board->getPiece(potentialMove);
		Color target_color = (target) ? target->getColor() : none;

		bool checkSameColor = (target_color != m_color) || calculateThreats;
		if (checkSameColor) {
			moves->push_back({ this, m_position, potentialMove });
		}
	}
}

Piece* Pawn::copy(Board* newBoard) {
	return new Pawn(m_color, m_position, newBoard);
}

void Pawn::possibleMoves(std::vector<Move>* moves, bool calculateThreats = false) {
	// Normal move
	int8_t pawnMoveDisplacement = m_color * 8;
	uint8_t singleMove = m_position + pawnMoveDisplacement;
	if (!m_board->getPiece(singleMove)) {
		moves->push_back({ this, m_position, singleMove });
		// Pawn power
		uint8_t doubleMove = singleMove + pawnMoveDisplacement;
		Piece* destinationOccupant = m_board->getPiece(doubleMove);
		if (m_canDoubleMove && !destinationOccupant) {
			moves->push_back({this, m_position, doubleMove});
		}
	}

	uint8_t currentFile = m_position % 8;
	uint8_t currentRank = m_position / 8;

	uint8_t leftFile = currentFile - 1;
	uint8_t rightFile = currentFile + 1;
	uint8_t attackRank = currentRank + m_color;
	uint8_t promotionRank = (m_color == black) ? 0 : 7;
	if (leftFile != -1 && currentRank != promotionRank) {
		uint8_t target =  attackRank * 8 + leftFile;
 		Piece* p = m_board->getPiece(target);
		if(p && p->getColor() == (m_color * -1) || calculateThreats) moves->push_back({this, m_position, target});
	}
	if (rightFile != 8 && currentRank != promotionRank) {
		uint8_t target = attackRank * 8 + rightFile;
		Piece* p = m_board->getPiece((target));
		if(p && p->getColor() == (m_color * -1) || calculateThreats) moves->push_back({this, m_position, target});
	}
}
