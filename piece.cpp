#include "piece.h"

#define IN_RANGE(x, a, b) (x >= a && x <= b)

Piece::Piece(Color c, glm::vec2 square, Board* b) {
	m_color    = c;
	m_position = square;
	m_board    = b;
	m_selected = false;
}

bool Piece::check4check(glm::vec2 move) {
	Board* testingMove = new Board(m_board);
	Piece* testingPiece = testingMove->getPiece(BIDX(m_position));
	testingMove->move(testingPiece, move);
	bool inCheck = testingMove->isInCheck(m_color);
	delete testingMove;
	return inCheck;
}

Piece* Knight::copy(Board* newBoard) {
	return new Knight(m_color, m_position, newBoard);
}

vec2s* Knight::legalMoves(bool calculateThreats = false) {
	// Vectors of move directions for the knight
	vec2s knightMoves = {
		{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, 
		 {1, -2},  {1, 2},  {2, -1},  {2, 1}
	};

	// Check each potential move for legality
	vec2s* moveSquares = new vec2s;
	for (int i = 0; i < knightMoves.size(); i++) {
		glm::vec2 potentialMove = m_position + knightMoves[i];
		if (!ON_BOARD(potentialMove.x) || !ON_BOARD(potentialMove.y)) continue;
		Piece* target = m_board->getPiece(BIDX(glm::vec2(potentialMove.x, potentialMove.y)));
		Color target_color = (target) ? target->getColor() : none;

		bool inbounds = IN_RANGE(potentialMove.x, 0, 7) && IN_RANGE(potentialMove.y, 0, 7);
		bool checkSameColor = (target_color != m_color) || calculateThreats;
		if (inbounds && checkSameColor && !check4check(potentialMove)) {
			moveSquares->push_back(potentialMove);
		}
	}

	return moveSquares;
}

Piece* Bishop::copy(Board* newBoard) {
	return new Bishop(m_color, m_position, newBoard);
}

vec2s* Bishop::legalMoves(bool calculateThreats = false) {
	int f; int r; Piece* q_pieceHere;

	vec2s* moveSquares = new vec2s;
	// We assume north to be in the positive rank direction (ie towards rank 8)
	// NW
	f = (int)m_position.x - 1; r = (int)m_position.y + 1;
	while (f >= 0 && r < 8) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2(f, r)));
		if (!q_pieceHere && !check4check({f, r})) moveSquares->push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({f, r})) { moveSquares->push_back({f, r}); break; }
		f--; r++;
	}

	// NE
	f = (int)m_position.x + 1; r = (int)m_position.y + 1;
	while (f < 8 && r < 8) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2(f, r)));
		if (!q_pieceHere && !check4check({ f, r })) moveSquares->push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({ f, r })) { moveSquares->push_back({f, r}); break; }
		f++; r++;
	}

	// SW
	f = (int)m_position.x - 1; r = (int)m_position.y - 1;
	while (f >= 0 && r >= 0) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2(f, r)));
		if (!q_pieceHere && !check4check({ f, r })) moveSquares->push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({ f, r })) { moveSquares->push_back({f, r}); break; }
		f--; r--;
	}

	// SE
	f = (int)m_position.x + 1; r = (int)m_position.y - 1;
	while (f < 8 && r >= 0) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2(f, r)));
		if (!q_pieceHere && !check4check({ f, r })) moveSquares->push_back({f, r});
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({ f, r })) { moveSquares->push_back({f, r}); break; }
		f++; r--;
	}

	return moveSquares;
}

Piece* Rook::copy(Board* newBoard) {
	return new Rook(m_color, m_position, newBoard);
}

vec2s* Rook::legalMoves(bool calculateThreats = false) {
	int f; int r; Piece* q_pieceHere;

	vec2s* moveSquares = new vec2s;

	// Left
	for (f = (int)m_position.x - 1; f >= 0; f--) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2(f, (int)m_position.y)));
		if (!q_pieceHere && !check4check({ f, (int)m_position.y })) moveSquares->push_back({ f, (int)m_position.y });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({ f, (int)m_position.y })) { moveSquares->push_back({ f, (int)m_position.y }); break; }
	}

	// Right
	for (f = (int)m_position.x + 1; f < 8; f++) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2(f, (int)m_position.y)));
		if (!q_pieceHere && !check4check({ f, (int)m_position.y })) moveSquares->push_back({ f, (int)m_position.y });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({ f, (int)m_position.y })) { moveSquares->push_back({ f, (int)m_position.y }); break; }
	}

	// Up
	for (r = (int)m_position.y + 1; r < 8; r++) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2((int)m_position.x, r)));
		if (!q_pieceHere && !check4check({ (int)m_position.x, r })) moveSquares->push_back({ (int)m_position.x, r });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({ (int)m_position.x, r })) { moveSquares->push_back({ (int)m_position.x, r }); break; }
	}

	// Down
	for (r = (int)m_position.y - 1; r >= 0; r--) {
		q_pieceHere = m_board->getPiece(BIDX(glm::vec2((int)m_position.x, r)));
		if (!q_pieceHere && !check4check({ (int)m_position.x, r })) moveSquares->push_back({ (int)m_position.x, r });
		else if (q_pieceHere->getColor() == m_color && !calculateThreats) break;
		else if (!check4check({ (int)m_position.x, r })) { moveSquares->push_back({ (int)m_position.x, r }); break; }
	}

	return moveSquares;
}

Piece* Queen::copy(Board* newBoard) {
	return new Queen(m_color, m_position, newBoard);
}

vec2s* Queen::legalMoves(bool calculateThreats = false) {
	Bishop testBishop = Bishop(m_color, m_position, m_board);
	Rook testRook = Rook(m_color, m_position, m_board);
	
	vec2s* moveSquaresBishop = testBishop.legalMoves();
	vec2s* moveSquaresRook = testRook.legalMoves();

	vec2s* moveSquares = new vec2s;
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

vec2s* King::legalMoves(bool calculateThreats = false) {
	vec2s kingMoves = {
		{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
		 {1, -1},  {1, 0},  {1, 1},  {0, 1}
	};

	vec2s* moveSquares = new vec2s;
	for (int i = 0; i < kingMoves.size(); i++) {
		glm::vec2 potentialMove = m_position + kingMoves[i];
		if (!ON_BOARD(potentialMove.x) || !ON_BOARD(potentialMove.y)) continue;
		Piece* target = m_board->getPiece(BIDX(potentialMove));
		Color target_color = (target) ? target->getColor() : none;

		bool inbounds = IN_RANGE(potentialMove.x, 0, 7) && IN_RANGE(potentialMove.y, 0, 7);
		bool checkSameColor = (target_color != m_color) || calculateThreats;
		if (inbounds && checkSameColor && !check4check(potentialMove)) {
			moveSquares->push_back(potentialMove);
		}
	}

	if (m_color == white) {
		if (m_board->canCastle(WHITE_SHORT) && !check4check({ 6, 0 })) {
			 moveSquares->push_back({ 6, 0 });
		}
		if (m_board->canCastle(WHITE_LONG) && !check4check({ 2, 0 })) {
			 moveSquares->push_back({ 2, 0 });
		}
	}
	if (m_color == black) {
		if (m_board->canCastle(BLACK_SHORT) && !check4check({ 6, 7 })) {
			moveSquares->push_back({ 6, 7 });
		}
		if (m_board->canCastle(BLACK_LONG) && !check4check({ 2, 7 })) {
			moveSquares->push_back({ 2, 7 });
		}
	}

	return moveSquares;
}

Piece* Pawn::copy(Board* newBoard) {
	return new Pawn(m_color, m_position, newBoard);
}

vec2s* Pawn::legalMoves(bool calculateThreats = false) {
	vec2s* moveSquares = new vec2s;

	int passant_rank = (m_color == white) ? 4 : 3;

	// Normal move
	glm::vec2 singleMove = { m_position.x, m_position.y + m_color };
	if (!m_board->getPiece(BIDX(singleMove)) && !check4check(singleMove)) {
		moveSquares->push_back(singleMove);
		// Pawn power
		glm::vec2 doubleMove = singleMove + glm::vec2(0, m_color);
		Piece* destinationOccupant = m_board->getPiece(BIDX(doubleMove));
		if (m_canDoubleMove && !destinationOccupant && !check4check(doubleMove)) {
			moveSquares->push_back(doubleMove);
		}
	}

	int leftFile = m_position.x - 1;
	int rightFile = m_position.x + 1;
	int rank = m_position.y + m_color;
	if (leftFile != 0) {
		glm::vec2 target = { leftFile, rank };
 		Piece* p = m_board->getPiece(BIDX(target));
		if(p && p->getColor() == (m_color * -1) && !check4check(target) || calculateThreats) moveSquares->push_back(target);
	}
	if (rightFile != 7) {
		glm::vec2 target = glm::vec2(rightFile, rank);
		Piece* p = m_board->getPiece(BIDX(target));
		if(p && p->getColor() == (m_color * -1) && !check4check(target) || calculateThreats) moveSquares->push_back(target);
	}

	// En passant TODO: move up above and track en passants using square, not file
	if (rank == passant_rank) {
		int f = m_board->getPassantFile(m_color);
		if ((f == m_position.x - 1 || f == m_position.x + 1) && !check4check({f, rank + m_color})) moveSquares->push_back({f, rank + m_color});
	}

	return moveSquares;
}
