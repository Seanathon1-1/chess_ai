#pragma once
#include "board.h"
#include "Texture.h"
#include <vector>



constexpr uint8_t WHITE_SHORT_CASTLE = 0x1;
constexpr uint8_t WHITE_LONG_CASTLE = 0x2;
constexpr uint8_t BLACK_SHORT_CASTLE = 0x4;
constexpr uint8_t BLACK_LONG_CASTLE = 0x8;
constexpr uint8_t WHITE_CHECK = 0x10;
constexpr uint8_t BLACK_CHECK = 0x20;
constexpr uint8_t PROMOTING = 0x40;
constexpr uint8_t WHOSE_TURN = 0x80;


class Game {
protected:
	Board* board = nullptr;

	int promotionSubject = -1;

	uint8_t gameStatus = 0xF;
	// Where are the kings
	uint8_t whiteKing;
	uint8_t blackKing;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	int8_t enPassantSquare = -1;

	bool isWaitingOnPromotion() const { return gameStatus & PROMOTING; }
	bool canCastle(Castling) const;
	Color whoseTurn() const;
	void makeLegalMove(Piece*, uint8_t);
	bool isInCheck(Color) const;
	void updateThreatMaps();
	void updateChecks();
	bool move(Piece*, uint8_t);
	void handlePromotion();
	bool hasLegalMove(Color);
	std::vector<uint8_t>* getLegalPieceMoves(Piece*, bool);
	bool check4check(Piece*, uint8_t, bool = false);
	void checkIfGameEnded();


	template<class T>
	void promote() {
		if (promotionSubject == -1) {
			std::cerr << "No piece able to promote!";
			return;
		}

		Piece* toDelete = board->getPiece(promotionSubject);
		Piece* promotedPiece = createPiece<T>(toDelete->getColor(), toDelete->getPosition(), board);
		board->placePiece(promotedPiece, promotionSubject);
		promotionSubject = -1;
		gameStatus &= ~PROMOTING;
		delete toDelete;

		gameStatus ^= WHOSE_TURN;
	}
public:
	Game();
	Game(Game*);
	~Game();

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
	Piece* held = nullptr;


	bool isHolding() { return (held != nullptr); }
	void printBoardImage();
	void grab(Piece*);
	Piece* drop();
	void handlePromotion();
public:
	GraphicalGame(unsigned int);
	~GraphicalGame();
	void createPromotionTextures();
	void deletePromotionTextures();
	void render();

	template<class T>
	void promote() {
		Game::promote<T>();
		deletePromotionTextures();
	}
};