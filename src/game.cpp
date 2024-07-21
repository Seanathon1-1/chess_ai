#include "game.h"
#include "square.h"
#include "piece.h"
#include "Player.h"
#include "imgui.h"
#include "Texture.h"

#define XTRC_BIT(map, bit) (((map) >> (bit)) & 1ULL)

constexpr float SQUARE_SIZE = .25f;
constexpr uint16_t BOARD_SIZE = 400;

bool operator==(const Move left, const Move right) { return left.piece == right.piece && left.source == right.source && left.target == right.target; }

// Sets a single bit in a 64-bit map to 1
void static setBit(uint64_t* map, uint8_t bit) {
	if (bit >= 64) {
		return;
	}
	*map |= (1ULL << bit);
}

Game::Game() {
	board = new Board(this);
	whiteKing = 4;
	blackKing = 60;
}

Game::Game(Game* base) {
	board = new Board(base->board);

	gameStatus = base->gameStatus;
	whiteKing = base->whiteKing;
	blackKing = base->blackKing;
	white_threat_map = base->white_threat_map;
	black_threat_map = base->black_threat_map;
	enPassantSquare = base->enPassantSquare;
}

Game::~Game() {
	if (board) delete board;
}

int8_t Game::getPlayStatus() const {
	return gameStatus & PLAY_STATUS;
}

void Game::makePlayerMove(Move& move) {
	if (makeLegalMove(move)) handlePromotion();
}

void Game::getAllLegalMoves(std::vector<Move>* moves, Color player) {
	Piece* nextPiece;
	for (int i = 0; i < 64; i++) {
		nextPiece = board->getPiece(i);
		if (!nextPiece || nextPiece->getColor() != player) continue;
		if (nextPiece->getColor() == none) std::cerr << nextPiece->getPosition() << std::endl;
		getLegalPieceMoves(moves, nextPiece);
	}
}

bool Game::canCastle(Castling whichCastle) const {
	bool transitCheck;
	bool currentCheck;
	bool spaceClear;
	switch (whichCastle) {
	case WHITE_SHORT:
		spaceClear = !board->getPiece(5) && !board->getPiece(6);
		transitCheck = XTRC_BIT(black_threat_map, whiteKing + 1);
		currentCheck = gameStatus & WHITE_CHECK;
		return (!currentCheck && !transitCheck && spaceClear);
	case WHITE_LONG:
		spaceClear = !board->getPiece(1) && !board->getPiece(2) && !board->getPiece(3);
		transitCheck = XTRC_BIT(black_threat_map, whiteKing - 1);
		currentCheck = gameStatus & WHITE_CHECK;
		return (!currentCheck && !transitCheck && spaceClear);
	case BLACK_SHORT:
		spaceClear = !board->getPiece(61) && !board->getPiece(62);
		transitCheck = XTRC_BIT(white_threat_map, blackKing + 1);
		currentCheck = gameStatus & BLACK_CHECK;
		return (!currentCheck && !transitCheck && spaceClear);
	case BLACK_LONG:
		spaceClear = !board->getPiece(57) && !board->getPiece(58) && !board->getPiece(59);
		transitCheck = XTRC_BIT(white_threat_map, blackKing - 1);
		currentCheck = gameStatus & BLACK_CHECK;
		return (!currentCheck && !transitCheck && spaceClear);
	}
	return false;
}

Color Game::whoseTurn() const {
	if (gameStatus & WHOSE_TURN) return black;
	return white;
}

bool Game::isInCheck(Color c) const {
	return c == black && (gameStatus & BLACK_CHECK) || c == white && (gameStatus & WHITE_CHECK);
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::updateThreatMaps()
*
* Description: Sets the threat map for both players. A threat map tells which squares of the board are covered
*              by a color's pieces and is used in calculating check/checkmate
\*-------------------------------------------------------------------------------------------------------------*/
void Game::updateThreatMaps() {
	white_threat_map = 0ULL;
	black_threat_map = 0ULL;

	Piece* piece;
	uint64_t* threat_map;
 	for (int i = 0; i < 64; i++) {
		std::vector<Move> moves;
		piece = board->getPiece(i);
		if (!piece) continue;

		threat_map = (piece->getColor() == white) ? &white_threat_map : &black_threat_map;
		getLegalPieceMoves(&moves, piece, true);

		for (Move move : moves) {
			setBit(threat_map, move.target);
		}
	}
}

bool Game::check4check(Move move, bool calculateThreats) {
	Game testingGame = Game(this);
	Piece* testingPiece = testingGame.board->getPiece(move.piece->getPosition());
	Move testingMove = { testingPiece, move.source, move.target };
	testingGame.makeMove(testingMove);
	if (!calculateThreats) testingGame.updateChecks();
	bool inCheck = testingGame.isInCheck(move.piece->getColor());
	return inCheck;
}

void Game::getLegalPieceMoves(std::vector<Move>* moves, Piece* piece, bool calculateThreats) {
	std::vector<Move> possibleMoves;
	piece->possibleMoves(&possibleMoves, calculateThreats);
	// Add castling if this is a king
	if (instanceof<King>(piece)) {
		if (piece->getColor() == white) {
			if (canCastle(WHITE_SHORT)) {
				possibleMoves.push_back({ piece, piece->getPosition(), 6 });
			}
			if (canCastle(WHITE_LONG)) {
				possibleMoves.push_back({ piece, piece->getPosition(), 2 });
			}
		}
		if (piece->getColor() == black) {
			if (canCastle(BLACK_SHORT)) {
				possibleMoves.push_back({ piece, piece->getPosition(), 62 });
			}
			if (canCastle(BLACK_LONG)) {
				possibleMoves.push_back({ piece, piece->getPosition(), 58 });
			}
		}
	}

	// Add en passant if this is a pawn
	if (instanceof<Pawn>(piece)) {
		uint8_t leftAttack = piece->getPosition() + (piece->getColor() * 8 - 1);
		uint8_t rightAttack = piece->getPosition() + (piece->getColor() * 8 + 1);
		if (leftAttack == enPassantSquare || rightAttack == enPassantSquare) {
			possibleMoves.push_back({ piece, piece->getPosition(), (uint8_t)enPassantSquare });
		}
	}

	for (Move move : possibleMoves) {
		if (!check4check(move, calculateThreats)) moves->push_back(move);
	}
}

void Game::checkIfGameEnded() {
	// Look for checkmate/stalemate
	Color nextPlayer = (whoseTurn() == white) ? black : white;
	std::vector<Move> nextPlayerMoves;
	getAllLegalMoves(&nextPlayerMoves, nextPlayer);

	// Check for when no moves are left
	if (nextPlayerMoves.size() == 0) {
		// Checkmate
		if (nextPlayer == white && gameStatus & WHITE_CHECK) {
			std::cout << "0-1" << std::endl;
			gameStatus |= BLACK_WIN;
			return;
		}
		else if (nextPlayer == black && gameStatus & BLACK_CHECK) {
			std::cout << "1-0" << std::endl;
			gameStatus |= WHITE_WIN;
			return;
		}

		// Stalemate
		else {
			std::cout << ".5-.5" << std::endl;
			gameStatus |= DRAW;
			return;
		}
	}

	// Check for insufficient material
	uint8_t numWhiteKnights = 0;
	uint8_t numWhiteBishops = 0;
	uint8_t numBlackKnights = 0;
	uint8_t numBlackBishops = 0;
	for (int i = 0; i < 64; i++) {
		Piece* piece = board->getPiece(i);
		if (!piece) continue;
		if (instanceof<Rook>(piece) || instanceof<Queen>(piece) || instanceof<Pawn>(piece)) break;
		else if (instanceof<Bishop>(piece)) (piece->getColor() == white) ? numWhiteBishops++ : numBlackBishops++;
		else if (instanceof<Knight>(piece)) (piece->getColor() == white) ? numWhiteKnights++ : numBlackKnights++;
		if (numWhiteBishops >= 2 || numBlackBishops >= 2) break;
		if (numWhiteKnights >= 3 || numBlackKnights >= 3) break;
		if (numWhiteKnights >= 1 && numWhiteBishops >= 1) break;
		if (numBlackKnights >= 1 && numBlackBishops >= 1) break;

		// All pieces have been checked and insufficient material has been found
		if (i == 63) {
			gameStatus |= DRAW;
			return;
		}
	}

	// Draw by fifty move rule
	if (fiftyMoveRule >= 100) {
		gameStatus |= DRAW;
		return;
	}
}


bool Game::hasLegalMove(Color c) {
	for (int i = 0; i < 64; i++) {
		Piece* p = board->getPiece(i % 8, i / 8);
		if (!p) continue;
		if (p->getColor() == c) {
			std::vector<Move> moves;
			getLegalPieceMoves(&moves, p, false);
			if (moves.size() != 0) return true;
		}
	}
	return false;
}

void Game::updateChecks() {
	// Look for check
	gameStatus &= ~(WHITE_CHECK | BLACK_CHECK);
	updateThreatMaps();
	if (XTRC_BIT(white_threat_map, blackKing)) gameStatus |= BLACK_CHECK;
	if (XTRC_BIT(black_threat_map, whiteKing)) gameStatus |= WHITE_CHECK;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::makeLegalMove(Piece, int, int)
*
* Parameters: p - Piece that is moving
*             src - Index of the square FROM which the Piece is moving
*             dest - Index of the square TO which the Piece is moving
* Description: Makes the move on the board and accordingly updates the game state, such as castling availablity,
*              en passant opportunities, whose turn, etc.
\*-------------------------------------------------------------------------------------------------------------*/
bool Game::makeLegalMove(Move move) {
	bool updatePassant = false;

	fiftyMoveRule++;

	// Enforce castling restrictions
	if (instanceof<King>(move.piece)) {
		if (move.piece->getColor() == white) {
			gameStatus &= ~(WHITE_SHORT_CASTLE | WHITE_LONG_CASTLE);
		}
		if (move.piece->getColor() == black) {
			gameStatus &= ~(BLACK_SHORT_CASTLE | BLACK_LONG_CASTLE);
		}
	}
	else if (instanceof<Rook>(move.piece)) {
		if (move.piece->getColor() == white) {
			if (move.source == 7) gameStatus &= ~WHITE_SHORT_CASTLE;
			if (move.source == 0) gameStatus &= ~WHITE_LONG_CASTLE;
		}
		if (move.piece->getColor() == black) {
			if (move.source == 63) gameStatus &= ~BLACK_SHORT_CASTLE;
			if (move.source == 56) gameStatus &= ~BLACK_LONG_CASTLE;
		}
	}
	else if (instanceof<Pawn>(move.piece)) {
		fiftyMoveRule = 0;
		updatePassant = true;
		((Pawn*)move.piece)->losePower();
		if (move.target == enPassantSquare) {
			board->passantCapture(enPassantSquare - move.piece->getColor() * 8);
		}
	}

	enPassantSquare = -1;

	// Check if en passant is available for next move
	if (updatePassant) {
		int dist = abs(move.target / 8 - move.source / 8);
		int enPassantTarget = move.target - (move.piece->getColor() * 8);
		if (dist == 2) enPassantSquare = enPassantTarget;
	}

	char sourceFile = 'a' + move.source % 8;
	char sourceRank = '1' + move.source / 8;
	char targetFile = 'a' + move.target % 8;
	char targetRank = '1' + move.target / 8;
	bool captureOccured = board->getPiece(move.target);
	std::string moveString = "";
	if (whoseTurn() == white) moveString += std::to_string(moveList.size() / 2 + 1) + ". ";
	if (!instanceof<Pawn>(move.piece)) moveString += move.piece->textboardSymbol();
	else if (captureOccured) moveString += 'a' + move.source % 8;
	if (captureOccured) {
		moveString += "x";
		fiftyMoveRule = 0;
	}
	moveString += targetFile;
	moveString += targetRank;
	moveList.push_back(moveString);

	if (makeMove(move)) {
		handlePromotion();
		return true;
	}

	
	updateChecks();
	if ((gameStatus & WHITE_CHECK && whoseTurn() == black) || (gameStatus & BLACK_CHECK && whoseTurn() == white)) moveList[moveList.size() - 1] += "+";


	checkIfGameEnded();
	if (((gameStatus & PLAY_STATUS) == WHITE_WIN) || ((gameStatus & PLAY_STATUS) == BLACK_WIN)) {
		moveList[moveList.size() - 1].pop_back();
		moveList[moveList.size() - 1] += "#";
	}

	// Other player's turn
	gameStatus ^= WHOSE_TURN;
	return false;
}

bool Game::makeMove(Move move) {
	if (!move.piece) return false;
	// Clear old spot
	board->makeMove(move.piece, move.source, move.target);

	// Extra move on castle
	if (instanceof<King>(move.piece)) {
		uint8_t targetFile = move.target % 8;
		uint8_t sourceFile = move.source % 8;
		if (move.piece->getColor() == white) {
			whiteKing = move.target;
			if (targetFile == 6 && abs(sourceFile - targetFile) == 2) {
				board->makeMove(board->getPiece(7, 0), 7, 5);
			}
			if (targetFile == 2 && abs(sourceFile - targetFile) == 2) {
				board->makeMove(board->getPiece(0, 0), 0, 3);
			}
		}
		if (move.piece->getColor() == black) {
			blackKing = move.target;
			if (targetFile == 6 && abs(sourceFile - targetFile) == 2) {
				board->makeMove(board->getPiece(7, 7), 63, 61);

			}
			if (targetFile == 2 && abs(sourceFile - targetFile) == 2) {
				board->makeMove(board->getPiece(0, 7), 56, 59);
			}
		}
	}

	// Check for promotion
	int promotion_sqr = (move.piece->getColor() == white) ? 7 : 0;
	if (instanceof<Pawn>(move.piece) && (move.target / 8 == promotion_sqr)) {
		promotionSubject = move.target;
		return 1;
	}
	return 0;
}

void Game::handlePromotion() {
	gameStatus |= PROMOTING;
}

GraphicalGame::GraphicalGame(unsigned int fbo) : Game(), fbo(fbo) {
	glGenTextures(1, &boardGraphic);
	glBindTexture(GL_TEXTURE_2D, boardGraphic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);


	unsigned int render_buffer_object;
	glGenRenderbuffers(1, &render_buffer_object);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attaching render buffer 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, boardGraphic, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer failed.\n" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	colorShader = new Shader("res/shaders/default.vert", "res/shaders/default.frag");
	pieceShader = new Shader("res/shaders/piece.vert", "res/shaders/piece.frag");
}

GraphicalGame::~GraphicalGame() {
	deletePromotionTextures();
}

void GraphicalGame::createPromotionTextures() {
	queenPromotion = new Texture(&Queen(whoseTurn(), 0, 0));
	rookPromotion = new Texture(&Rook(whoseTurn(), 0, 0));
	knightPromotion = new Texture(&Knight(whoseTurn(), 0, 0));
	bishopPromotion = new Texture(&Bishop(whoseTurn(), 0, 0));
}

void GraphicalGame::deletePromotionTextures() {
	if (queenPromotion) delete queenPromotion; 
	if (rookPromotion) delete rookPromotion;
	if (knightPromotion) delete knightPromotion;
	if (bishopPromotion) delete bishopPromotion;

	queenPromotion = rookPromotion = knightPromotion = bishopPromotion = 0;
}

void GraphicalGame::grab(Piece* p) {
	held = p;
	p->select();
}

Piece* GraphicalGame::drop() {
	Piece* returnPiece = held;
	held->deselect();
	held = nullptr;
	return returnPiece;
}

void GraphicalGame::handlePromotion() {
	Game::handlePromotion();
	createPromotionTextures();
}


#define FIX_BOARD_POSITION
void GraphicalGame::printBoardImage() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
#ifdef FIX_BOARD_POSITION
	ImGui::SetNextWindowPos(ImVec2(WIN_WIDTH - BOARD_SIZE, 0));
	ImGui::SetNextWindowSize(ImVec2(BOARD_SIZE, BOARD_SIZE));
#endif
	if (ImGui::Begin("Gameview", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {		//ImGui::SetCursorPos({0, 0})
		glm::vec3 topLeft;
		Piece* p;
		for (int i = 0; i < 64; i++) {
			float left = (i % 8 - 4) * .25f;
			float top = (float)(i / -8 + 4) * .25f;
			topLeft = glm::vec3(left, top, 0);
			bool isLightSquare = (i % 2) ^ (i / 8 % 2);
			p = board->getPiece(i);
			Square s = Square(topLeft, isLightSquare, p);
			s.draw(colorShader);

			if (p && !p->isSelected()) {
				s.drawTexture(pieceShader);
			}
		}
		if (held) {
			ImVec2 mPos = ImGui::GetMousePos();
			topLeft.x = (mPos.x - (WIN_WIDTH - BOARD_SIZE)) / BOARD_SIZE * 2 - SQUARE_SIZE / 2 - 1;
			topLeft.y = mPos.y / BOARD_SIZE * 2 + SQUARE_SIZE / 2 - 1;
			topLeft.z = -.1f;
			p = held;
			Square s = Square(topLeft, 0, p);
			s.drawTexture(pieceShader);
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Image((void*)(intptr_t)boardGraphic, ImGui::GetContentRegionAvail());
	}
	ImGui::PopStyleVar();
	ImGui::End();
}


void GraphicalGame::printMoveList() {
	if (ImGui::Begin("Move List")) {
		if (ImGui::BeginListBox("##", { 200, 300 })) {
			ImGui::Selectable("Starting Position", false);
			if (ImGui::BeginTable("Move List", 2, ImGuiTableFlags_Borders, ImVec2(190.f, 0.f))) {
				int i = 1;
				for (std::string move : moveList) {
					ImGui::TableNextColumn();
					ImGui::Selectable(move.c_str(), false);
					i++;
				}
			}
			ImGui::EndTable();
			if ((gameStatus & PLAY_STATUS) == WHITE_WIN) ImGui::Text("1-0");
			if ((gameStatus & PLAY_STATUS) == BLACK_WIN) ImGui::Text("0-1");
			if ((gameStatus & PLAY_STATUS) == DRAW)		 ImGui::Text(".5-.5");
		}
		ImGui::EndListBox();
	}
	ImGui::End();
}

/*-------------------------------------------------------------------------------------------------------------*\
* GraphicalGame::render()
* 
* Description: Sets up the ImGUI context of our game, handles user entered moves, and handles pawn promotion
\*-------------------------------------------------------------------------------------------------------------*/
void GraphicalGame::render() {
	printBoardImage();
	printMoveList();
	(whoseTurn() == white) ? whitePlayer->itsMyTurn() : blackPlayer->itsMyTurn();

	ImGui::Begin("Gameview");
	ImGuiIO& io = ImGui::GetIO();

	if ((gameStatus & PLAY_STATUS) != PLAYING) {
		ImGui::End();
		return;
	}

	if (isWaitingOnPromotion()) {
		PieceType promotionPiece = open;
		ImGui::Begin("Promotion Selection", 0, ImGuiWindowFlags_NoTitleBar);

		if (ImGui::ImageButton("Queen", queenPromotion->getTextureData(), ImVec2(70, 70))) {
			promotionPiece = queen;
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("Rook", rookPromotion->getTextureData(), ImVec2(70, 70))) {
			promotionPiece = rook;
		}

		if (ImGui::ImageButton("Knight", knightPromotion->getTextureData(), ImVec2(70, 70))) {
			promotionPiece = knight;
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("Bishop", bishopPromotion->getTextureData(), ImVec2(70, 70))) {
			promotionPiece = bishop;
		}

		switch (promotionPiece) {
		case queen:
			promote<Queen>();
			break;
		case rook:
			promote<Rook>();
			break;
		case knight:
			promote<Knight>();
			break;
		case bishop:
			promote<Bishop>();
			break;
		}

		ImGui::End();
	}

	else if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !isHolding()) {
		ImVec2 wPos  = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos  = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file    = (char)(mPos.x / (wSize.x / 8));
		char rank    = (char)(8 - mPos.y / (wSize.y / 8));
		Piece* p     = board->getPiece(rank * 8 + file);
		if (p && p->getColor() == whoseTurn() && !isWaitingOnPromotion()) {
			grab(p);
		}
	} 

	else if (ImGui::IsMouseReleased(0) && isHolding()) {
		ImVec2 wPos			= ImGui::GetWindowPos();
		ImVec2 wSize		= ImGui::GetWindowSize();
		ImVec2 mPos			= { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file			= (char)(mPos.x / (wSize.x / 8));
		char rank			= (char)(8 - mPos.y / (wSize.y / 8));
		Piece* targetPiece	= board->getPiece(rank * 8 + file);
		Piece* movedPiece	= drop();
		if (movedPiece) {
			std::vector<Move> legals;
			getLegalPieceMoves(&legals, movedPiece, false);
			Move attempt = { movedPiece, movedPiece->getPosition(), (uint8_t)(rank * 8 + file) };
			if (std::find(legals.begin(), legals.end(), attempt) != legals.end()) {
				if (makeLegalMove(attempt)) handlePromotion();
			}
		}
	}
	ImGui::End();
}

void GraphicalGame::addPlayer(Player* player, Color color) {
	if (color == white) whitePlayer = player;
	else blackPlayer = player;
}


