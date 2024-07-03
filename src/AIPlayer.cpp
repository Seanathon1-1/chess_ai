#include "Player.h"

void AIPlayer::pickLegalMove() {
	std::vector<Move> legalMoves;
	activeGame->getAllLegalMoves(&legalMoves, playerColor);
	Move move = legalMoves.at(rand() % legalMoves.size());
	activeGame->makePlayerMove(move);
}