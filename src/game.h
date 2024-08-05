#pragma once
#include "board.h"
#include "Texture.h"
#include <vector>


// Masks for game status
constexpr uint16_t WHITE_SHORT_CASTLE	= 0x001;
constexpr uint16_t WHITE_LONG_CASTLE	= 0x002;
constexpr uint16_t BLACK_SHORT_CASTLE	= 0x004;
constexpr uint16_t BLACK_LONG_CASTLE	= 0x008;
constexpr uint16_t WHITE_CHECK			= 0x010;
constexpr uint16_t BLACK_CHECK			= 0x020;
constexpr uint16_t PROMOTING			= 0x040;
constexpr uint16_t WHOSE_TURN			= 0x080;
constexpr uint16_t PLAY_STATUS			= 0x300;

// PLAY_STATUS values
constexpr uint16_t PLAYING		= 0x000;
constexpr uint16_t WHITE_WIN	= 0x100;
constexpr uint16_t BLACK_WIN	= 0x200;
constexpr uint16_t DRAW			= 0x300;

class Player;
class HumanPlayer;
class AIPlayer;
struct Move;


class Game {
protected:
	friend Move;
	std::shared_ptr<Board> board = nullptr;
	std::vector<std::string> moveList;

	int promotionSubject = -1;

	uint16_t gameStatus = 0xF;
	// Where are the kings
	uint8_t whiteKing;
	uint8_t blackKing;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	uint8_t fiftyMoveRule = 0;
	int8_t enPassantSquare = -1;

	bool isWaitingOnPromotion() const { return gameStatus & PROMOTING; }
	bool canCastle(Castling) const;
	Color whoseTurn() const;
	bool makeMove(Move);
	bool makeLegalMove(Move);
	void updateChecks();
	void handlePromotion();
	bool hasLegalMove(Color);
	void getLegalPieceMoves(std::vector<Move>*, std::shared_ptr<Piece>, bool = false);
	bool isInCheck(Color) const;
	bool check4check(Move, bool = false);
	void updateThreatMaps();
	void checkIfGameEnded();


	template<class T>
	void promote() {
		if (promotionSubject == -1) {
			std::cerr << "No piece able to promote!";
			return;
		}

		std::shared_ptr<Piece> toDelete = board->getPiece(promotionSubject);
		std::shared_ptr<Piece> promotedPiece = createPiece<T>(toDelete->getColor(), toDelete->getPosition());
		board->placePiece(promotedPiece, promotionSubject);
		promotionSubject = -1;
		gameStatus &= ~PROMOTING;

		std::string promotionMove = moveList[moveList.size() - 1];
		moveList.pop_back();
		moveList[moveList.size() - 1] = promotionMove + "=" + promotedPiece->textboardSymbol();
		gameStatus ^= WHOSE_TURN;
	}
public:
	Game();
	Game(Game*);
	Game(std::shared_ptr<Game>);

	int8_t getPlayStatus() const;
	void makePlayerMove(Move&);
	void getAllLegalMoves(std::vector<Move>*, Color);
	float getMaterialDifference();
};

class GraphicalGame : public Game {
	unsigned int fbo = 0;
	Shader* colorShader = nullptr;
	Shader* pieceShader = nullptr;

	GLuint boardGraphic = 0;
	Texture* queenPromotion = 0;
	Texture* rookPromotion = 0;
	Texture* knightPromotion = 0;
	Texture* bishopPromotion = 0;
	std::shared_ptr<Piece> held = nullptr;

	Player* whitePlayer = 0;
	Player* blackPlayer = 0;


	bool isHolding() { return (held != nullptr); }
	void printBoardImage();
	void printMoveList();
	void grab(std::shared_ptr<Piece>);
	std::shared_ptr<Piece> drop();
	void handlePromotion();
	void createPromotionTextures();
	void deletePromotionTextures();
public:
	GraphicalGame(unsigned int);
	~GraphicalGame();
	void render();
	void addPlayer(Player*, Color);

	template<class T>
	void promote() {
		Game::promote<T>();
		deletePromotionTextures();
	}
};


struct Move {
	std::shared_ptr<Piece> piece;
	uint8_t source;
	uint8_t target;

	Move() { Move(nullptr, 0, 0); }
	Move(Game& game, uint8_t s, uint8_t t) : source(s), target(t) {
		piece = game.board->getPiece(s);
	}
	Move(std::shared_ptr<Piece> p, uint8_t s, uint8_t t) : piece(p), source(s), target(t) {}
};
extern bool operator==(const Move left, const Move right);

struct MoveHasher {
	size_t operator()(const Move& move) const {
		return std::hash<int>()(move.source) ^ std::hash<int>()(move.target);
	}
};
