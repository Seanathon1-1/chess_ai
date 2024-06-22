#include "board.h"
#include "piece.h"
#include "game.h"
#include "imgui.h"
#include "gl/glew.h"
#include <cctype>
#include "VAO.h"
#include "EBO.h"
#include "square.h"

#define SQUARE_SIZE .25f
#define BOARD_SIZE 400

const char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };



/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(GLuint)
* 
* Parameters: fbo - ID of the FrameBuffer Object that the board will be drawn onto
* Description: Creates a new Board with the classical starting position
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(GLuint fbo, Game* game) : fbo(fbo), game(game) {
 	memset(&board, 0, sizeof(Piece*) * 64);
	// Pawns
	for (int file = 0; file < 8; file++) {
		board[8 + file]  = createPiece<Pawn>(white, 8 + file, this);
		board[48 + file] = createPiece<Pawn>(black, 48 + file, this);
	}

	// Rooks
	board[ 0] = createPiece<Rook>(white, 0, this);
	board[ 7] = createPiece<Rook>(white, 7, this);
	board[56] = createPiece<Rook>(black, 56, this);
	board[63] = createPiece<Rook>(black, 63, this);

	// Knights
	board[ 1] = createPiece<Knight>(white, 1, this);
	board[ 6] = createPiece<Knight>(white, 6, this);
	board[57] = createPiece<Knight>(black, 57, this);
	board[62] = createPiece<Knight>(black, 62, this);

	// Bishop
	board[ 2] = createPiece<Bishop>(white, 2, this);
	board[ 5] = createPiece<Bishop>(white, 5, this);
	board[58] = createPiece<Bishop>(black, 58, this);
	board[61] = createPiece<Bishop>(black, 61, this);

	// Queens
	board[ 3] = createPiece<Queen>(white, 3, this);
	board[59] = createPiece<Queen>(black, 59, this);

	// Kings
	board[ 4] = createPiece<King>(white, 4, this);
	board[60] = createPiece<King>(black, 60, this);

	glGenTextures(1, &gBoard);
	glBindTexture(GL_TEXTURE_2D, gBoard);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBoard, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer failed.\n" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	colorShader = new Shader("res/shaders/default.vert", "res/shaders/default.frag");
	pieceShader = new Shader("res/shaders/piece.vert", "res/shaders/piece.frag");

	whose_turn = white;
	white_king = 4;
	black_king = 60;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(Board*)
* 
* Parameters: base - Pointer to the board that will be copied
* Description: Makes a new board which is a copy of 'base'. Used to test moves
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(Board* base) {
	for (int i = 0; i < 64; i++) {
		if (base->board[i]) board[i] = (base->board[i])->copy(this);
		else board[i] = nullptr;
	}
	gameStatus			= base->gameStatus;
	white_king			= base->white_king;
	black_king			= base->black_king;
	whose_turn			= base->whose_turn;
	white_threat_map	= base->white_threat_map;
	black_threat_map	= base->black_threat_map;
	white_en_passant	= base->white_en_passant;
	black_en_passant	= base->black_en_passant;
}

// Performs cleanup before deleting the board
Board::~Board() {
	if (pieceShader) delete pieceShader;
	if (colorShader) delete colorShader;
	for (Piece* p : board) if (p) delete p;
	glDeleteTextures(1, &gBoard);
}

/*-------------------------------------------------------------------------------------------------------------*\
* print_threatmap(uint64_t)
* 
* Parameters: map - 64 bit representation of a threatmap
* Description: Prints out an 8x8 ASCII representation of a threatmap to the console
\*-------------------------------------------------------------------------------------------------------------*/
void print_threatmap(uint64_t map) {
	std::cout << HORZ_LINE;
	glm::ivec2 sqr;
	for (int rank = 7; rank >= 0; rank--) {
		std::cout << "| ";
		for (int file = 0; file < 8; file++) {
			sqr = glm::ivec2( file, rank );
			std::cout << (char)(XTRC_BIT(map, sqr.y * 8 + sqr.x) + '0');
			std::cout << " | ";
		}
		std::cout << "\b\n" << HORZ_LINE;
	}
	std::cout << "\n";
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::makeMove(Piece, int, int)
* 
* Parameters: p - Piece that is being moved
*             s - Index of the source square
*             d - Index of the destination square
* Description: Handles moving a piece on the board and checks if we need to promote a pawn
* Return Value: True if we need to promote a pawn, false otherwise
\*-------------------------------------------------------------------------------------------------------------*/
bool Board::makeMove(Piece* p, int s, int d) {
	// Make the move
	board[d] = p;
	board[s] = nullptr;

	return false;
}


/*-------------------------------------------------------------------------------------------------------------*\
* Board::printBoard(std::string&)
* 
* Description: Outputs a string represention of the current state of the board
* Return: String representation of the board to be printed
\*-------------------------------------------------------------------------------------------------------------*/
std::string Board::printBoardString() const {
	Piece* p;
	char piece_c;
	std::string s = HORZ_LINE;
	for (int rank = 7; rank >= 0; rank--) {
		s += "| ";
		for (int file = 0; file < 8; file++) {
			p = getPiece(file, rank);
			if (p) piece_c = (p->getColor() == white) ? tolower(p->textboardSymbol()) : p->textboardSymbol();
			else piece_c = ' ';
			s += piece_c;
			s += " | ";
		}
		s += "\n";
		s += HORZ_LINE;
	}
	return s;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::printBoardImage(Shader*)
* 
* Description Prints out the graphical representation of the board.
\*-------------------------------------------------------------------------------------------------------------*/
void Board::printBoardImage() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::SetNextWindowPos(ImVec2(WIN_WIDTH - BOARD_SIZE, 0));
	ImGui::SetNextWindowSize(ImVec2(BOARD_SIZE, BOARD_SIZE));
	if (ImGui::Begin("Gameview", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {		//ImGui::SetCursorPos({0, 0})
		glm::vec3 topLeft;
		Piece* p;
		for (int i = 0; i < 64 ; i++) {
			float left = (i % 8 - 4) * .25f;
			float top = (i / -8 + 4) * .25f;
			topLeft = glm::vec3(left, top, 0.f);
			bool isLightSquare = (i % 2) ^ (i / 8 % 2);
			p = board[i];
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

		ImGui::Image((void*)(intptr_t)gBoard, ImGui::GetContentRegionAvail());
	}
	ImGui::PopStyleVar();
	ImGui::End(); 
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::render()
* 
* Description: Handles how the board will be rendered. Calls for both the graphical and text based renderings.
\*-------------------------------------------------------------------------------------------------------------*/
void Board::render() {
	std::string board_string = printBoardString();
	ImGui::Begin("Play window");
	ImGui::Text(board_string.c_str());
	ImGui::End();
	
	printBoardImage(); 
}

bool Board::canCastle(Castling whichCastle) const {
	bool transitCheck;
	bool currentCheck;
	switch (whichCastle) {
	case WHITE_SHORT:
		transitCheck = XTRC_BIT(black_threat_map, white_king + 1);
		currentCheck = gameStatus & WHITE_CHECK;
		return (!currentCheck && !transitCheck);
	case WHITE_LONG:
		transitCheck = XTRC_BIT(black_threat_map, white_king - 1);
		currentCheck = gameStatus & WHITE_CHECK;
		return (!currentCheck && !transitCheck);
	case BLACK_SHORT:
		transitCheck = XTRC_BIT(white_threat_map, black_king + 1);
		currentCheck = gameStatus & BLACK_CHECK;
		return (!currentCheck && !transitCheck);
	case BLACK_LONG:
		transitCheck = XTRC_BIT(white_threat_map, black_king - 1);
		currentCheck = gameStatus & BLACK_CHECK;
		return (!currentCheck && !transitCheck);
	}
	return false;
}

int Board::getPassantFile(Color attacking) const {
	return (attacking == white) ? black_en_passant : white_en_passant;
}


// Sets a single bit in a 64-bit map to 1
void set_bit(uint64_t* map, uint8_t bit) {
	if (bit >= 64) {
		return;
	}
	*map |= (1ULL << bit);
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::updateThreatMaps()
*
* Description: Sets the threat map for both players. A threat map tells which squares of the board are covered
*              by a color's pieces and is used in calculating check/checkmate
\*-------------------------------------------------------------------------------------------------------------*/
void Board::updateThreatMaps() {
	white_threat_map = 0ULL;
	black_threat_map = 0ULL;

	Piece* piece;
	std::vector<uint8_t>* squares;
	uint64_t* threat_map;
	for (int i = 0; i < 64; i++) {
		piece = board[i];
		if (!piece) continue;
		
		threat_map = (piece->getColor() == white) ? &white_threat_map : &black_threat_map;
		squares = getLegalPieceMoves(piece, true);

		for (uint8_t sqr : *squares) {
			set_bit(threat_map, sqr);
		}
		delete squares;
	}
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::makeUserMove(std::string)
*
* Parameters: move - String representation of the move we want to make. Must be of form a1b2.
* Description: Checks that a move is legal then makes the move and checks if the game is over.
\*-------------------------------------------------------------------------------------------------------------*/
void Board::makeUserMove(std::string move) {
	// Moves should only be 4 characters long
	if (move.length() != 4) {
		std::cout << "Could not understand move, please enter a move like e2e4.\n";
		return;
	}

	// Check that moves are in bounds
	bool skip = 0;
	for (int i = 0; i < 4; i++) {
		int val = (i % 2 == 0) ? move[i] - 'a' : move[i] - '1';
		if (!ON_BOARD(val)) {
			std::cout << "Move out of bounds, moves should fit within a1-h8.\n";
			std::cerr << "move[" << i << "]: " << val << std::endl;
			skip = 1;
			break;
		}
	}
	if (skip) return;

	int sf = move[0] - 'a';
	int sr = move[1] - '1';
	int df = move[2] - 'a';
	int dr = move[3] - '1';

	// Check that there is a piece on the initial square
	Piece* selected = getPiece(sf, sr);
	if (!selected) {
		std::cout << "No piece selected to move.\n";
		return;
	}

	// Check that it is the correct player's turn
	if (selected->getColor() != whoseTurn()) {
		std::cout << "Can't move other player's piece.\n";
		return;
	}

	// Check if the destination is a legal move
	uint8_t destination = dr * 8 + df;
	std::vector<uint8_t>* moves_possible = selected->legalMoves(false);
	bool move_exists = 0;
	for (uint8_t move : *moves_possible) if (move == destination) { move_exists = 1; break; }
	if (!move_exists) {
		std::cout << "Move is not legal.\n";
		return;
	}
	delete moves_possible;

	// Makes the move and changes whose turn it is
	makeLegalMove(selected, destination);

	// Look for checkmate/stalemate
	if (hasLegalMove(whoseTurn())) {
		// Checkmate
		if (whoseTurn() == white && gameStatus & WHITE_CHECK) {
			std::cout << "0-1" << std::endl;
			return;
		}
		else if (whoseTurn() == black && gameStatus & BLACK_CHECK) {
			std::cout << "1-0" << std::endl;
			return;
		}

		// Stalemate
		else {
			std::cout << ".5-.5" << std::endl;
			return;
		}
	}
}

std::vector<uint8_t>* Board::getLegalPieceMoves(Piece* piece, bool calculateThreats) {
	std::vector<uint8_t>* possibleMoves = piece->legalMoves(calculateThreats);
	std::vector<uint8_t>* legalMoves = new std::vector<uint8_t>();
	for (auto move : *possibleMoves) {
		if (!piece->check4check(move, calculateThreats)) legalMoves->push_back(move);
	}
	delete possibleMoves;
	return legalMoves;
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
void Board::makeLegalMove(Piece* p, uint8_t target) {
	
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
	} else if (instanceof<Rook>(p)) {
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
		uint8_t enPassantCaptureSquare = (p->getColor() == white) ? 40 + black_en_passant : 16 + white_en_passant;
		if (target == enPassantCaptureSquare) {
			clearSquare(enPassantCaptureSquare - p->getColor() * 8);
		}
	}

	white_en_passant = -1;
	black_en_passant = -1;
	
	// Check if en passant is available for next move
	if (updatePassant) {
		int dist = abs(target / 8 - src / 8);
		int pawn_file = src % 8;
		if (dist == 2) (p->getColor() == white) ? white_en_passant = pawn_file : black_en_passant = pawn_file;
	}
	 
	if (move(p, target)) {
		gameStatus |= PROMOTING;
		game->createPromotionTextures();
		return;
	}

	updateChecks();

	// Other player's turn
	if (whose_turn == white) whose_turn = black;
	else whose_turn = white;
}

bool Board::hasLegalMove(Color c) const {
	for (int i = 0; i < 64; i++) {
		Piece* p = getPiece(i % 8, i / 8);
		if (!p) continue;
		if (p->getColor() == c) {
			if (p->legalMoves()->size() != 0) return true;
		}
	}
	return false;
}

bool Board::isInCheck(Color c) const {
	return c == black && (gameStatus & BLACK_CHECK) || c == white && (gameStatus & WHITE_CHECK);
}

void Board::grab(Piece* p) { 
	held = p;
	p->select();

}

Piece* Board::drop() {
	Piece* returnPiece = held;
	held->deselect();
	held = nullptr;
	return returnPiece;
}

void Board::placePiece(Piece* piece, int file, int rank) {
	placePiece(piece, rank * 8 + file );
}

void Board::placePiece(Piece* piece, uint8_t square) {
	board[square] = piece;
	if (piece) piece->place(square);
}

void Board::clearSquare(int file, int rank) {
	placePiece(nullptr, rank * 8 + file);
}

void Board::clearSquare(uint8_t s) {
	placePiece(nullptr, s);
}

void Board::updateChecks() {
	// Look for check
	gameStatus &= ~(WHITE_CHECK | BLACK_CHECK);
	updateThreatMaps();
	if (XTRC_BIT(white_threat_map, black_king)) gameStatus |= BLACK_CHECK;
	if (XTRC_BIT(black_threat_map, white_king)) gameStatus |= WHITE_CHECK;
}


bool Board::move(Piece* piece, uint8_t square) {
	if (!piece) return false;
	// Clear old spot
	uint8_t origin = piece->getPosition();
	clearSquare(origin);

	// Move to new spot
	placePiece(piece, square);

	// Extra move on castle
	if (instanceof<King>(piece)) {
		uint8_t squareFile = square % 8;
		uint8_t originFile = origin % 8;
		if (piece->getColor() == white) {
			white_king = square;
			if (squareFile == 6 && abs(originFile - squareFile) == 2) {
				placePiece(getPiece(7, 0), 5, 0);
				clearSquare(7, 0);
			}
			if (squareFile == 2 && abs(originFile - squareFile) == 2) {
				placePiece(getPiece(0, 0), 3, 0);
				clearSquare(0, 0);
			}
		}
		if (piece->getColor() == black) {
			black_king = square;
			if (squareFile == 6 && abs(originFile - squareFile) == 2) {
				placePiece(getPiece(7, 7), 5, 7);
				clearSquare(7, 7);
			}
			if (squareFile == 2 && abs(originFile - squareFile) == 2) {
				placePiece(getPiece(0, 7), 3, 7);
				clearSquare(0, 7);
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