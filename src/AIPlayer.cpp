#include "Player.h"
#include "piece.h"
#include <unordered_map>


constexpr uint16_t PICK_RANDOM_MOVE = 8;


class MCTNode {
	Game* nodeState;
	MCTNode* parent;
	std::unordered_map<Move, MCTNode*, MoveHasher> children;
	Color whoseMove;
	int16_t totalScore;
	uint16_t numRollouts;
	bool fullyExpanded;

public:
	MCTNode(Game* game, MCTNode* parent, int color) {
		whoseMove = (color == -1) ? black : white;
		nodeState = game;
		this->parent = parent;
		
		totalScore = 0;
		numRollouts = 0;
		fullyExpanded = false;
	}
	~MCTNode() {
		delete nodeState;
		for (auto child : children) {
			delete child.second;
		}
		children.clear();
	}

	MCTNode* getParent() { return parent; }

	MCTNode* expand() {
		std::vector<Move> possibleMoves;
		nodeState->getAllLegalMoves(&possibleMoves, whoseMove);
		do {
			int pickAMove = rand() % possibleMoves.size();
			Move working = possibleMoves.at(pickAMove);
			if (children.find(working) == children.end()) {
				Game* newChildGame = new Game(nodeState);
				newChildGame->makePlayerMove(working);
				MCTNode* newChild = new MCTNode(newChildGame, this, whoseMove * -1);
				children.insert({ working, newChild });
				return newChild;
			}
			possibleMoves.erase(possibleMoves.begin() + pickAMove);
		} while (possibleMoves.size() > 0);
		fullyExpanded = true;
		return 0;
	}

	uint8_t simulate() {
		Game simulationGame = Game(nodeState);
		std::vector<Move> possibleMoves;
		Color whoseTurn = whoseMove;
		while (simulationGame.getPlayStatus() == PLAYING) {
			simulationGame.getAllLegalMoves(&possibleMoves, whoseTurn);
			if (possibleMoves.size() == 0) break;
			Move nextMove = possibleMoves[rand() % possibleMoves.size()];
			simulationGame.makePlayerMove(nextMove);
			whoseTurn = (whoseTurn == black) ? white : black;
			possibleMoves.clear();
		} 
		if (simulationGame.getPlayStatus() == WHITE_WIN) return 1;
		if (simulationGame.getPlayStatus() == BLACK_WIN) return -1;
		return 0;
	}

	void updateScore(int8_t result) {
		totalScore += result;
		numRollouts++;
	}
	
	bool isLeaf() { 
		return children.size() == 0; 
	}
	
	float getScore() {
		if (numRollouts == 0) return 0.f;
		return (float)totalScore / (float)numRollouts; 
	}
	
	MCTNode* bestChild() {
		MCTNode* bestNode = children.begin()->second;
		float bestScore = bestNode->getScore();
		for (std::pair<Move, MCTNode*> child : children) {
			if (whoseMove == white) {
				if (child.second->getScore() > bestScore) {
					bestNode = child.second;
					bestScore = child.second->getScore();
				}
			}
			else if (whoseMove == black) {
				if (child.second->getScore() < bestScore) {
					bestNode = child.second;
					bestScore = child.second->getScore();
				}
			}
		}
		return bestNode;
	}
};


class MonteCarloTree {
	MCTNode* root;
	MCTNode* activeNode;

	void treeSelection() {
		MCTNode* workingNode = root;
		while (!workingNode->isLeaf()) {
			if (rand() % 1000 < PICK_RANDOM_MOVE) break;
			workingNode = workingNode->bestChild();
		}
		activeNode = workingNode;
	}

	uint8_t treeExpansionSimulation() {
		activeNode = activeNode->expand();
		if(activeNode) return activeNode->simulate();
		return 2;
	}

	void treeBackpropogation(int8_t result) {
		while (1) {
			activeNode->updateScore(result);
			activeNode = activeNode->getParent();
			if (activeNode == root) return;
		}
	}

public:
	MonteCarloTree(MCTNode* root) {
		this->root = root;
		this->activeNode = 0;
	}
	~MonteCarloTree() {
		delete root;
	}

	void search(uint32_t resourceCount) {
		for (uint32_t i = 0; i < resourceCount; i++) {
			treeSelection();
			int8_t result = treeExpansionSimulation();
			if (result == 2) continue;
			treeBackpropogation(result);
		}
	}
};


//#define RANDOM_MOVE

void AIPlayer::itsMyTurn() {
#ifdef RANDOM_MOVE
	std::vector<Move> legalMoves;
	activeGame->getAllLegalMoves(&legalMoves, playerColor);
	if (legalMoves.size() == 0) return;
	Move move = legalMoves.at(rand() % legalMoves.size());
	activeGame->makePlayerMove(move);
#else
	MCTNode root = MCTNode(activeGame, 0, playerColor);
	MonteCarloTree findingNextMove = MonteCarloTree(&root);
	findingNextMove.search(10);
#endif
}  