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

bool Piece::check4check(uint8_t move, bool calculateThreats) {
	Board* testingMove = new Board(m_board);
	Piece* testingPiece = testingMove->getPiece(m_position);
	testingMove->move(testingPiece, move);
	if (!calculateThreats) testingMove->updateChecks();
	bool inCheck = testingMove->isInCheck(m_color);
	delete testingMove;
	return inCheck;
}

Piece* Knight::copy(Board* newBoard) {
	return new Knight(m_color, m_position, newBoard);
}

std::vector<uint8_t>* Knight::legalMoves(bool calculateThreats = false) {
	// Vectors of move directions for the knight
	std::vector<int8_t> knightMoves = { -17, -15, -10, -6, 6, 10, 15, 17 };

	// Check each potential move for legality
	std::vector<uint8_t>* moveSquares = new std::vector<uint8_t>();
	for (int i = 0; i < knightMoves.size(); i++) {
		int8_t moveRankOffset = lround(knightMoves[i] / 8.f);
		int8_t moveFileOffset = knightMoves[i] - moveRankOffset * 8;
		int8_t potentialMoveFile = m_position % 8 + moveFileOffset;
		int8_t potentialMoveRank = m_position / 8 + moveRankOffset;

		if (!ON_BOARD(potentialMoveFile) || !ON_BOARD(potentialMoveRank)) continue;
		uint8_t potentialMove = m_position + knightMoves[i];
		Piece* target = m_board->getPiece(potentialMove);
		Color target_color = (target) ? target->getColor() : none;

		bool checkSameColor = (target_color != m_color) || calculateThreats;
		if (checkSameColor) {
			moveSquares->push_back(potentialMove);
		}
	}

	return moveSquares;
}

Piece* Bishop::copy(Board* newBoard) {
	return new Bishop(m_color, m_position, newBoard);
}

std::vector<uint8_t>* Bishop::legalMoves(bool calculateThreats = false) {
	int8_t f; int8_t r; Piece* q_pieceHere;

	std::vector<uint8_t>* moveSquares = new std::vector<uint8_t>();
	uint8_t currentFile = m_position % 8;
	uint8_t currentRank = m_position / 8;
	uint8_t moveSquare;

	// We assume north to be in the positive rank direction (ie towards rank 8)
	// NW
	f = currentFile - 1; r = currentRank + 1;
	while (f >= 0 && r < 8) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
		f--; r++;
	}

	// NE
	f = currentFile + 1; r = currentRank + 1;
	while (f < 8 && r < 8) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
		f++; r++;
	}

	// SW
	f = currentFile - 1; r = currentRank - 1;
	while (f >= 0 && r >= 0) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
		f--; r--;
	}

	// SE
	f = currentFile + 1; r = currentRank - 1;
	while (f < 8 && r >= 0) {
		moveSquare = r * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
		f++; r--;
	}

	return moveSquares;
}

Piece* Rook::copy(Board* newBoard) {
	return new Rook(m_color, m_position, newBoard);
}

std::vector<uint8_t>* Rook::legalMoves(bool calculateThreats = false) {
	int f; int r; Piece* q_pieceHere;

	std::vector<uint8_t>* moveSquares = new std::vector<uint8_t>();
	uint8_t currentFile = m_position % 8;
	uint8_t currentRank = m_position / 8;
	uint8_t moveSquare;

	// Left
	for (f = currentFile - 1; f >= 0; f--) {
		moveSquare = currentRank * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
	}

	// Right
	for (f = currentFile; f < 8; f++) {
		moveSquare = currentRank * 8 + f;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
	}

	// Up
	for (r = currentRank + 1; r < 8; r++) {
		moveSquare = r * 8 + currentFile;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
	}

	// Down
	for (r = currentRank - 1; r >= 0; r--) {
		moveSquare = r * 8 + currentFile;
		q_pieceHere = m_board->getPiece(moveSquare);
		if (!q_pieceHere) moveSquares->push_back(moveSquare);
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else { moveSquares->push_back(moveSquare); break; }
	}

	return moveSquares;
}

Piece* Queen::copy(Board* newBoard) {
	return new Queen(m_color, m_position, newBoard);
}

std::vector<uint8_t>* Queen::legalMoves(bool calculateThreats = false) {
	Bishop testBishop = Bishop(m_color, m_position, m_board);
	Rook testRook = Rook(m_color, m_position, m_board);
	
	std::vector<uint8_t>* moveSquaresBishop = testBishop.legalMoves();
	std::vector<uint8_t>* moveSquaresRook = testRook.legalMoves();

	std::vector<uint8_t>* moveSquares = new std::vector<uint8_t>();
	moveSquares->reserve(moveSquaresBishop->size() + moveSquaresRook->size());
	moveSquares->insert(moveSquares->end(), moveSquaresBishop->begin(), moveSquaresBishop->end());
	moveSquares->insert(moveSquares->end(), moveSquaresRook->begin(), moveSquaresRook->end());


	delete moveSquaresBishop;
	delete moveSquaresRook;
	return moveSquares;
}

Piece* King::copy(Board* newBoard) {
	return new King(m_color, m_position, newBoard);
}

std::vector<uint8_t>* King::legalMoves(bool calculateThreats = false) {
	std::vector<int8_t> kingMoves = { -9, -8, -7, -1, 1, 7, 8, 9 };

	std::vector<uint8_t>* moveSquares = new std::vector<uint8_t>();
	for (int i = 0; i < kingMoves.size(); i++) {
		int8_t moveRankOffset = lround(kingMoves[i] / 8.f);
		int8_t moveFileOffset = kingMoves[i] - moveRankOffset * 8;
		int8_t potentialMoveFile = m_position % 8 + moveFileOffset;
		int8_t potentialMoveRank = m_position / 8 + moveRankOffset;

		if (!ON_BOARD(potentialMoveFile) || !ON_BOARD(potentialMoveRank)) continue;
		uint8_t potentialMove = m_position + kingMoves[i];
		Piece* target = m_board->getPiece(potentialMove);
		Color target_color = (target) ? target->getColor() : none;

		bool checkSameColor = (target_color != m_color) || calculateThreats;
		if (checkSameColor) {
			moveSquares->push_back(potentialMove);
		}
	}

	if (m_color == white) {
		if (m_board->canCastle(WHITE_SHORT)) {
			 moveSquares->push_back(6);
		}
		if (m_board->canCastle(WHITE_LONG)) {
			 moveSquares->push_back(2);
		}
	}
	if (m_color == black) {
		if (m_board->canCastle(BLACK_SHORT)) {
			moveSquares->push_back(62);
		}
		if (m_board->canCastle(BLACK_LONG)) {
			moveSquares->push_back(58);
		}
	}

	return moveSquares;
}

Piece* Pawn::copy(Board* newBoard) {
	return new Pawn(m_color, m_position, newBoard);
}

std::vector<uint8_t>* Pawn::legalMoves(bool calculateThreats = false) { 
	std::vector<uint8_t>* moveSquares = new std::vector<uint8_t>();

	int passant_rank = (m_color == white) ? 4 : 3;

	// Normal move
	int8_t pawnMoveDisplacement = m_color * 8;
	uint8_t singleMove = m_position + pawnMoveDisplacement;
	if (!m_board->getPiece(singleMove)) {
		moveSquares->push_back(singleMove);
		// Pawn power
		uint8_t doubleMove = singleMove + pawnMoveDisplacement;
		Piece* destinationOccupant = m_board->getPiece(doubleMove);
		if (m_canDoubleMove && !destinationOccupant) {
			moveSquares->push_back(doubleMove);
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
		if(p && p->getColor() == (m_color * -1) || calculateThreats) moveSquares->push_back(target);
	}
	if (rightFile != 8 && currentRank != promotionRank) {
		uint8_t target = attackRank * 8 + rightFile;
		Piece* p = m_board->getPiece((target));
		if(p && p->getColor() == (m_color * -1) || calculateThreats) moveSquares->push_back(target);
	}

	// En passant TODO: move up above and track en passants using square, not file
	if (currentRank == passant_rank) {
		int f = m_board->getPassantFile(m_color);
		if ((f == currentFile - 1 || f == currentFile + 1) ) moveSquares->push_back(attackRank * 8 + f);
	}

	return moveSquares;
}
