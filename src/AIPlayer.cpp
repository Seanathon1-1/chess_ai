#include "Player.h"
#include "piece.h"
#include <unordered_map>
#include <memory>


constexpr uint16_t PICK_RANDOM_MOVE = 8;

/*
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
		nodeState = new Game(game);
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
				MCTNode* newChild = new MCTNode(nodeState, this, whoseMove * -1);
				newChild->nodeState->makePlayerMove(working);
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
*/

struct MMTNode {
	std::shared_ptr<Game> nodeState;
	MMTNode* parent;
	std::unordered_map<Move, MMTNode*, MoveHasher> children;
	Move bestMove;
	Color whoseMove;
	float eval;

	MMTNode(std::shared_ptr<Game> nodeState, MMTNode* parent, Color whoseMove) {
		this->nodeState = std::make_shared<Game>(Game(nodeState));
		this->parent = parent;
		this->whoseMove = whoseMove;
		eval = 0.f;

		bestMove = Move();
	}

	~MMTNode() {
		for (auto& child : children) {
			delete child.second;
			child.second = 0;
		}
		children.clear();
	}

	bool isLeaf() {
		return children.size() == 0;
	}

	/*
	void updateBestChild(MMTNode* child, Move childMove) {
		if (whoseMove == white && child->eval > bestChildEval) {
			bestChild = child;
			bestMove = childMove;
			bestChildEval = child->eval;
		}
		if (whoseMove == black && child->eval < bestChildEval) {
			bestChild = child;
			bestMove = childMove;
			bestChildEval = child->eval;
		}
	}
	*/

	void evaluate() {
		if (isLeaf()) eval = nodeState->getMaterialDifference();
		else {
			float bestChildEval = (whoseMove == white) ? -10000.f : 10000.f;
			for (auto& child : children) {
				if (child.second == 0) continue;
				if (whoseMove == white && child.second->eval > bestChildEval) {
					bestMove = child.first;
					bestChildEval = child.second->eval;
				}
				if (whoseMove == black && child.second->eval < bestChildEval) {
					bestMove = child.first;
					bestChildEval = child.second->eval;
				}
			}

			eval = bestChildEval;
		}
	}

	void expand() {
		std::vector<Move> legalMoves;
		nodeState->getAllLegalMoves(&legalMoves, whoseMove);
		Color nextPlayer = (whoseMove == white) ? black : white;
		for (Move childMove : legalMoves) {
			MMTNode* child = new MMTNode(nodeState, this, nextPlayer);
			child->nodeState->makePlayerMove(childMove);
			child->evaluate();
			//updateBestChild(child, childMove);
			children[childMove] = child;
		}
	}
};

class MiniMaxTree {
	MMTNode* root = 0;

public:
	MiniMaxTree(MMTNode* root) {
		this->root = root;
	}

	void search(uint64_t searchLimit) {
		assert(root != 0);
		for (int i = 0; i < searchLimit; i++) {
			MMTNode* working = root;
			MMTNode* next = 0;
			while (!working->isLeaf()) {
				next = working->children[working->bestMove];
				if (!next) break;
				working = next;
			}
			working->expand();
			while (working) {
				working->evaluate();
				working = working->parent;
			}
		}
	}
};


//#define RANDOM_MOVE
//#define MONTE_CARLO_TREE
#define MINIMAX_TREE

void AIPlayer::itsMyTurn() {
#ifdef MINIMAX_TREE
	MMTNode root = MMTNode(activeGame, 0, playerColor);
	MiniMaxTree findingNextMove = MiniMaxTree(&root);
	findingNextMove.search(10);
	Move nextMove = Move(*activeGame, root.bestMove.source, root.bestMove.target);
	activeGame->makePlayerMove(Move(nextMove));
#endif
}  