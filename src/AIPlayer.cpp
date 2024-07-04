#include "Player.h"
#include "piece.h"

void AIPlayer::itsMyTurn() {
	std::vector<Move> legalMoves;
	activeGame->getAllLegalMoves(&legalMoves, playerColor);
	Move move = legalMoves.at(rand() % legalMoves.size());
	if (move.piece->getColor() == none) std::cerr << "FUCK ON A StICK DOG\n";
	activeGame->makePlayerMove(move);
}