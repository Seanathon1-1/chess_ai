#include "game.h"
#include "square.h"
#include "piece.h"
#include "imgui.h"
#include "Texture.h"

#define XTRC_BIT(map, bit) (((map) >> (bit)) & 1ULL)

#define SQUARE_SIZE .25f
#define BOARD_SIZE 400

// Sets a single bit in a 64-bit map to 1
void setBit(uint64_t* map, uint8_t bit) {
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

bool Game::canCastle(Castling whichCastle) const {
	bool transitCheck;
	bool currentCheck;
	switch (whichCastle) {
	case WHITE_SHORT:
		transitCheck = XTRC_BIT(black_threat_map, whiteKing + 1);
		currentCheck = gameStatus & WHITE_CHECK;
		return (!currentCheck && !transitCheck);
	case WHITE_LONG:
		transitCheck = XTRC_BIT(black_threat_map, whiteKing - 1);
		currentCheck = gameStatus & WHITE_CHECK;
		return (!currentCheck && !transitCheck);
	case BLACK_SHORT:
		transitCheck = XTRC_BIT(white_threat_map, blackKing + 1);
		currentCheck = gameStatus & BLACK_CHECK;
		return (!currentCheck && !transitCheck);
	case BLACK_LONG:
		transitCheck = XTRC_BIT(white_threat_map, blackKing - 1);
		currentCheck = gameStatus & BLACK_CHECK;
		return (!currentCheck && !transitCheck);
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
	std::vector<uint8_t>* squares;
	uint64_t* threat_map;
	for (int i = 0; i < 64; i++) {
		piece = board->getPiece(i);
		if (!piece) continue;

		threat_map = (piece->getColor() == white) ? &white_threat_map : &black_threat_map;
		squares = getLegalPieceMoves(piece, true);

		for (uint8_t sqr : *squares) {
			setBit(threat_map, sqr);
		}
		delete squares;
	}
}

bool Game::check4check(Piece* piece, uint8_t move, bool calculateThreats) {
	Game testingMove = Game(this);
	Piece* testingPiece = testingMove.board->getPiece(piece->getPosition());
	testingMove.move(testingPiece, move);
	if (!calculateThreats) testingMove.updateChecks();
	bool inCheck = testingMove.isInCheck(piece->getColor());
	return inCheck;
}

std::vector<uint8_t>* Game::getLegalPieceMoves(Piece* piece, bool calculateThreats) {
	std::vector<uint8_t>* possibleMoves = piece->possibleMoves(calculateThreats);
	// Add castling if this is a king
	if (instanceof<King>(piece)) {
		if (piece->getColor() == white) {
			if (canCastle(WHITE_SHORT)) {
				possibleMoves->push_back(6);
			}
			if (canCastle(WHITE_LONG)) {
				possibleMoves->push_back(2);
			}
		}
		if (piece->getColor() == black) {
			if (canCastle(BLACK_SHORT)) {
				possibleMoves->push_back(62);
			}
			if (canCastle(BLACK_LONG)) {
				possibleMoves->push_back(58);
			}
		}
	}

	// Add en passant if this is a pawn
	if (instanceof<Pawn>(piece)) {
		uint8_t leftAttack = piece->getPosition() + (piece->getColor() * 8 - 1);
		uint8_t rightAttack = piece->getPosition() + (piece->getColor() * 8 + 1);
		if (leftAttack == enPassantSquare || rightAttack == enPassantSquare) {
			possibleMoves->push_back(enPassantSquare);
		}
	}

	std::vector<uint8_t>* legalMoves = new std::vector<uint8_t>();
	for (auto move : *possibleMoves) {
		if (!check4check(piece, move, calculateThreats)) legalMoves->push_back(move);
	}
	delete possibleMoves;
	return legalMoves;
}

void Game::checkIfGameEnded() {
	// Look for checkmate/stalemate
	Color nextPlayer = (whoseTurn() == white) ? black : white;
	if (!hasLegalMove(nextPlayer)) {
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
			gameStatus |= STALEMATE;
			return;
		}
	}
}


bool Game::hasLegalMove(Color c) {
	for (int i = 0; i < 64; i++) {
		Piece* p = board->getPiece(i % 8, i / 8);
		if (!p) continue;
		if (p->getColor() == c) {
			if (getLegalPieceMoves(p, false)->size() != 0) return true;
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
bool Game::makeLegalMove(Piece* p, uint8_t target) {

	uint8_t src = p->getPosition();
	bool updatePassant = false;

	// Enforce castling restrictions
	if (instanceof<King>(p)) {
		if (p->getColor() == white) {
			gameStatus &= ~(WHITE_SHORT_CASTLE | WHITE_LONG_CASTLE);
		}
		if (p->getColor() == black) {
			gameStatus &= ~(BLACK_SHORT_CASTLE | BLACK_LONG_CASTLE);
		}
	}
	else if (instanceof<Rook>(p)) {
		if (p->getColor() == white) {
			if (src == 7) gameStatus &= ~WHITE_SHORT_CASTLE;
			if (src == 0) gameStatus &= ~WHITE_LONG_CASTLE;
		}
		if (p->getColor() == black) {
			if (src == 63) gameStatus &= ~BLACK_SHORT_CASTLE;
			if (src == 56) gameStatus &= ~BLACK_LONG_CASTLE;
		}
	}
	else if (instanceof<Pawn>(p)) {
		updatePassant = true;
		((Pawn*)p)->losePower();
		if (target == enPassantSquare) {
			board->passantCapture(enPassantSquare - p->getColor() * 8);
		}
	}

	enPassantSquare = -1;

	// Check if en passant is available for next move
	if (updatePassant) {
		int dist = abs(target / 8 - src / 8);
		int enPassantTarget = target - (p->getColor() * 8);
		if (dist == 2) enPassantSquare = enPassantTarget;
	}

	char sourceFile = 'a' + src % 8;
	char sourceRank = '1' + src / 8;
	char targetFile = 'a' + target % 8;
	char targetRank = '1' + target / 8;
	bool captureOccured = board->getPiece(target);
	std::string moveString = "";
	if (whoseTurn() == white) moveString += std::to_string(moveList.size() / 2 + 1) + ". ";
	if (!instanceof<Pawn>(p)) moveString += p->textboardSymbol();
	else if (captureOccured) moveString += 'a' + src % 8;
	if (captureOccured) moveString += "x";
	moveString += targetFile;
	moveString += targetRank;
	moveList.push_back(moveString);

	if (move(p, target)) {
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

bool Game::move(Piece* piece, uint8_t square) {
	if (!piece) return false;
	// Clear old spot
	uint8_t origin = piece->getPosition();
	board->makeMove(piece, origin, square);

	// Extra move on castle
	if (instanceof<King>(piece)) {
		uint8_t squareFile = square % 8;
		uint8_t originFile = origin % 8;
		if (piece->getColor() == white) {
			whiteKing = square;
			if (squareFile == 6 && abs(originFile - squareFile) == 2) {
				board->makeMove(board->getPiece(7, 0), 7, 5);
			}
			if (squareFile == 2 && abs(originFile - squareFile) == 2) {
				board->makeMove(board->getPiece(0, 0), 0, 3);
			}
		}
		if (piece->getColor() == black) {
			blackKing = square;
			if (squareFile == 6 && abs(originFile - squareFile) == 2) {
				board->makeMove(board->getPiece(7, 7), 63, 61);

			}
			if (squareFile == 2 && abs(originFile - squareFile) == 2) {
				board->makeMove(board->getPiece(0, 7), 56, 59);
			}
		}
	}

	// Check for promotion
	int promotion_sqr = (piece->getColor() == white) ? 7 : 0;
	if (instanceof<Pawn>(piece) && (square / 8 == promotion_sqr)) {
		promotionSubject = square;
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
			topLeft = glm::vec3(left, top, 0.f);
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
			topLeft.x = mPos.x / BOARD_SIZE * 2 - SQUARE_SIZE / 2 - 3;
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

	ImGui::Begin("Gameview");
	ImGuiIO& io = ImGui::GetIO();

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
			std::vector<uint8_t>* legals = getLegalPieceMoves(movedPiece, false);
			uint8_t attempt = rank * 8 + file;
			if (std::find(legals->begin(), legals->end(), attempt) != legals->end()) {
				if (makeLegalMove(movedPiece, attempt)) handlePromotion();
			}
			delete legals;
		}
	}
	ImGui::End();
}


