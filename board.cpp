#include "board.h"
#include "piece.h"
#include "imgui.h"
#include "gl/glew.h"
#include <cctype>
#include "VAO.h"
#include "EBO.h"

#include <Windows.h>

#define BOARD_SIZE 400

// Colors for the squares of the board
#define LIGHT_SQUARE_COLOR glm::vec3(1.f)
#define DARK_SQUARE_COLOR glm::vec3(.1f, 0.f, .2f)

// Offsets to the corners of the board squares
#define SQUARE_SIZE .25f
glm::vec3 square_top_right_offset    = glm::vec3(SQUARE_SIZE, 0.f, 0.f);
glm::vec3 square_bottom_left_offset  = glm::vec3(0.f,  -1 * SQUARE_SIZE, 0.f);
glm::vec3 square_bottom_right_offset = glm::vec3(SQUARE_SIZE, -1 * SQUARE_SIZE, 0.f);

const char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };

/*-------------------------------------------------------------------------------------------------------------*\
* Square::draw(Shader*)
* 
* Parameters: shader - Pointer to shader used to draw our square.
* Description: Draws a solid colored square
\*-------------------------------------------------------------------------------------------------------------*/
void Square::draw(Shader* shader) {
	// Vertex data for this square
	glm::vec3 top_right_corner    = top_left_corner + square_top_right_offset;
	glm::vec3 bottom_left_corner  = top_left_corner + square_bottom_left_offset;
	glm::vec3 bottom_right_corner = top_left_corner + square_bottom_right_offset;
	glm::vec3 color               = (is_dark) ? DARK_SQUARE_COLOR : LIGHT_SQUARE_COLOR;

	// Vertex and index buffer data
	float vertices[] = {
		top_left_corner.x, top_left_corner.y, top_left_corner.z, color.x, color.y, color.z,
		top_right_corner.x, top_right_corner.y, top_right_corner.z, color.x, color.y, color.z,
		bottom_left_corner.x, bottom_left_corner.y, bottom_left_corner.z, color.x, color.y, color.z,
		bottom_right_corner.x, bottom_right_corner.y, bottom_right_corner.z, color.x, color.y, color.z
	};
	unsigned int indices[] = {0, 1, 2, 1, 2, 3};

	// Create opengl objects
	VAO* vertex_array = new VAO();
	vertex_array->bind();
	VBO* vertex_buffer = new VBO(vertices, sizeof(vertices));
	EBO* element_buffer = new EBO(indices, sizeof(indices));

	// Link the position and color attributes
	vertex_array->linkAttribute(vertex_buffer, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	vertex_array->linkAttribute(vertex_buffer, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// Draw
	shader->activate();
	vertex_array->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Cleanup
	shader->deactivate();
	vertex_array->unbind();
	vertex_buffer->unbind();
	element_buffer->unbind();
	delete vertex_array;
	delete vertex_buffer;
	delete element_buffer;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Square::drawTexture(Shader*)
* 
* Parameters: shader - Pointer to shader used to draw the piece texture within this square
* Description: Draws a chess piece over this square
\*-------------------------------------------------------------------------------------------------------------*/
void Square::drawTexture(Shader* shader) {
	// Vertex data for this square
	glm::vec3 top_right_corner    = top_left_corner + square_top_right_offset;
	glm::vec3 bottom_left_corner  = top_left_corner + square_bottom_left_offset;
	glm::vec3 bottom_right_corner = top_left_corner + square_bottom_right_offset;

	// Vertex and index buffer data 
	GLfloat vertices[] = {
		top_left_corner.x, top_left_corner.y, top_left_corner.z - .1f, 0.f, 1.f,
		top_right_corner.x, top_right_corner.y, top_right_corner.z - .1f, 1.f, 1.f,
		bottom_left_corner.x, bottom_left_corner.y, bottom_left_corner.z - .1f, 0.f, 0.f,
		bottom_right_corner.x, bottom_right_corner.y, bottom_right_corner.z - .1f, 1.f, 0.f
	};
	GLuint indices[] = { 0, 1, 2, 1, 2, 3 };

	// Create opengl elements
	VAO* vertex_array = new VAO();
	vertex_array->bind();
	VBO* vertex_buffer = new VBO(vertices, sizeof(vertices));
	EBO* element_buffer = new EBO(indices, sizeof(indices));

	// Link position and texture coordinate attributes
	vertex_array->linkAttribute(vertex_buffer, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	vertex_array->linkAttribute(vertex_buffer, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// setup opengl texture object 
	int width, height, nChannels;
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Get texture from file through stb image
	std::string texture_path = "../../../res/textures/";
	texture_path.append((piece->getColor() == black) ? "black_" : "white_");
	texture_path += piece->textboardSymbol();
	texture_path.append(".png");
	unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::cerr << "Fail!: " << texture_path << "\n";
		std::cerr << "Working Directory: " << std::string(buffer) << std::endl;
	}
	stbi_image_free(data);


	// Draw the piece
	shader->activate();
	glEnable(GL_BLEND);
	glUniform1i(glGetUniformLocation(shader->ID, "ourTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	vertex_array->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Cleanup
	shader->deactivate();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	vertex_array->unbind();
	vertex_buffer->unbind();
	element_buffer->unbind();
	glDeleteTextures(1, &texture);
	delete vertex_array;
	delete vertex_buffer;
	delete element_buffer;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(GLuint)
* 
* Parameters: fbo - ID of the FrameBuffer Object that the board will be drawn onto
* Description: Creates a new Board with the classical starting position
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(GLuint fbo) : fbo(fbo) {
 	memset(&board, 0, sizeof(Piece*) * 64);
	// Pawns
	for (int file = 0; file < 8; file++) {
		board[8 + file]  = new Pawn(white, { file, 1 }, this);
		board[48 + file] = new Pawn(black, { file, 6 }, this);
	}

	// Rooks
	board[ 0] = new Rook(white, { 0,0 }, this);
	board[ 7] = new Rook(white, { 7,0 }, this);
	board[56] = new Rook(black, { 0,7 }, this);
	board[63] = new Rook(black, { 7,7 }, this);

	// Knights
	board[ 1] = new Knight(white, { 1,0 }, this);
	board[ 6] = new Knight(white, { 6,0 }, this);
	board[57] = new Knight(black, { 1,7 }, this);
	board[62] = new Knight(black, { 6,7 }, this);

	// Bishop
	board[ 2] = new Bishop(white, { 2,0 }, this);
	board[ 5] = new Bishop(white, { 5,0 }, this);
	board[58] = new Bishop(black, { 2,7 }, this);
	board[61] = new Bishop(black, { 5,7 }, this);

	// Queens
	board[ 3] = new Queen(white, { 3,0 }, this);
	board[59] = new Queen(black, { 3,7 }, this);

	// Kings
	board[ 4] = new King(white, { 4,0 }, this);
	board[60] = new King(black, { 4,7 }, this);

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

	colorShader = new Shader("../../../res/shaders/default.vert", "../../../res/shaders/default.frag");
	pieceShader = new Shader("../../../res/shaders/piece.vert", "../../../res/shaders/piece.frag");

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
	white_check        = base->white_check;
	black_check        = base->black_check;
	white_king         = base->white_king;
	black_king         = base->black_king;
	whose_turn         = base->whose_turn;
	white_threat_map   = base->white_threat_map;
	black_threat_map   = base->black_threat_map;
	white_en_passant   = base->white_en_passant;
	black_en_passant   = base->black_en_passant;
	white_short_castle = base->white_short_castle;
	black_short_castle = base->black_short_castle;
	white_long_castle  = base->white_long_castle;
	black_long_castle  = base->black_long_castle;
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
	int sqr;
	for (int rank = 7; rank >= 0; rank--) {
		std::cout << "| ";
		for (int file = 0; file < 8; file++) {
			sqr = BIDX(glm::vec2( file, rank ));
			std::cout << (char)(XTRC_BIT(map, sqr) + '0');
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
* Board::promote(PieceType)
* 
* Parameters: type - Kind of piece the pawn is promoting to
* Description: Handles pawn promotion
\*-------------------------------------------------------------------------------------------------------------*/
template<class T>
void Board::promote() {
	if (promoting == -1) {
		std::cerr << "No piece able to promote!";
		return;
	}
	if (T != Queen && T != Rook && T != Bishop && T != Knight) {
		std::cerr << "Invalid promotion type!";
		return;
	}

	Pawn* toDelete = (Pawn*)board[promoting];
	board[promoting] = new T(toDelete->getColor(), toDelete->getPosition(), this);
	promoting = -1;
	delete toDelete;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::printBoard(std::string&)
* 
* Description: Outputs a string represention of the current state of the board
* Return: String representation of the board to be printed
\*-------------------------------------------------------------------------------------------------------------*/
std::string Board::printBoardString() {
	Piece* p;
	char piece_c;
	std::string s = HORZ_LINE;
	for (int rank = 7; rank >= 0; rank--) {
		s += "| ";
		for (int file = 0; file < 8; file++) {
			p = board[(int)BIDX(glm::vec2(file, rank))];
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
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::SetNextWindowPos(ImVec2(WIN_WIDTH - BOARD_SIZE, 0));
	ImGui::SetNextWindowSize(ImVec2(BOARD_SIZE, BOARD_SIZE));
	if (ImGui::Begin("Gameview", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {		//ImGui::SetCursorPos({0, 0})
		for (int i = 0; i < 64 ; i++) {
			float left = (i % 8 - 4) * .25f;
			float top = (i / -8 + 4) * .25f;
			glm::vec3 top_left = glm::vec3(left, top, 0.f);
			bool isBlack = (i % 2) ^ (i / 8 % 2);
			Piece* p = board[i];
			Square* s = new Square(top_left, isBlack, p);
			s->draw(colorShader);

			if (p) {
				if (p->isSelected()) {
					delete s;
					ImVec2 mPos = ImGui::GetMousePos();
					top_left.x = mPos.x / BOARD_SIZE * 2 - SQUARE_SIZE / 2 - 3;
					top_left.y = mPos.y / BOARD_SIZE * 2 + SQUARE_SIZE / 2 - 1;
					s = new Square(top_left, isBlack, p);
				}
				s->drawTexture(pieceShader);
			}
			delete s;
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.3f, 0.1f, 0.3f, 0.1f);
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

bool Board::canCastle(Castling whichCastle) {
	bool transit_check;
	switch (whichCastle) {
	case WHITE_SHORT:
		transit_check = XTRC_BIT(black_threat_map, white_king + 1);
		return (!white_check && !transit_check);
	case WHITE_LONG:
		transit_check = XTRC_BIT(black_threat_map, white_king - 1);
		return (!white_check && !transit_check);
	case BLACK_SHORT:
		transit_check = XTRC_BIT(white_threat_map, black_king + 1);
		return (!black_check && !transit_check);
	case BLACK_LONG:
		transit_check = XTRC_BIT(white_threat_map, black_king - 1);
		return (!black_check && !transit_check);
	}
	return false;
}

int Board::getPassantFile(Color attacking) {
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
	vec2s* squares = new vec2s;
	uint64_t* threat_map;
	for (int i = 0; i < 64; i++) {
		squares->clear();
		piece = board[i];
		if (!piece) continue;
		
		threat_map = (piece->getColor() == white) ? &white_threat_map : &black_threat_map;
		squares = piece->legalMoves(true);

		for (glm::vec2 sqr : *squares) {
			int index = BIDX(glm::vec2(sqr.x, sqr.y));
			set_bit(threat_map, index);
		}
	}
	delete squares;
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
	int select_idx = BIDX(glm::vec2(sf, sr));
	Piece* selected = getPiece(select_idx);
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
	int dest_idx = BIDX(glm::vec2(df, dr));
	vec2s* moves_possible = selected->legalMoves(false);
	bool move_exists = 0;
	for (glm::vec2 move : *moves_possible) if (BIDX(glm::vec2(move.x, move.y)) == dest_idx) { move_exists = 1; break; }
	if (!move_exists) {
		std::cout << "Move is not legal.\n";
		return;
	}
	delete moves_possible;

	// Makes the move and changes whose turn it is
	makeLegalMove(selected, { select_idx, dest_idx });

	// Look for checkmate/stalemate
	if (hasLegalMove(whoseTurn())) {
		// Checkmate
		if (whoseTurn() == white && white_check) {
			std::cout << "0-1" << std::endl;
			return;
		}
		else if (whoseTurn() == black && black_check) {
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

/*-------------------------------------------------------------------------------------------------------------*\
* Game::makeLegalMove(Piece, int, int)
*
* Parameters: p - Piece that is moving
*             src - Index of the square FROM which the Piece is moving
*             dest - Index of the square TO which the Piece is moving
* Description: Makes the move on the board and accordingly updates the game state, such as castling availablity,
*              en passant opportunities, whose turn, etc.
\*-------------------------------------------------------------------------------------------------------------*/
void Board::makeLegalMove(Piece* p, glm::vec2 target) {
	glm::vec2 src = p->getPosition();

	// Enforce castling restrictions
	if (instanceof<King>(p)) {
		if (p->getColor() == white) {
			white_short_castle = 0;
			white_long_castle = 0;
			white_king = BIDX(target);
		}
		if (p->getColor() == black) {
			black_short_castle = 0;
			black_long_castle = 0;
			black_king = BIDX(target);
		}
	} else if (instanceof<Rook>(p)) {
		if (p->getColor() == white) {
			if (src.x == 7 && src.y == 0) white_short_castle = 0;
			if (src.x == 0 && src.y == 0) white_long_castle = 0;
		}
		if (p->getColor() == black) {
			if (src.x == 7 && src.y == 7) black_short_castle = 0;
			if (src.x == 0 && src.y == 7) black_long_castle = 0;
		}
	} else if (instanceof<Pawn>(p)) {
		// Check if en passant is available for next move
		int dist = abs(target.y - src.y);
		int pawn_file = src.x;
		if (dist == 2) (p->getColor() == white) ? white_en_passant = pawn_file : black_en_passant = pawn_file;
	}

	wait_for_promote |= move(p, target);

	// Look for check
	updateThreatMaps();
	black_check = XTRC_BIT(white_threat_map, black_king);
	white_check = XTRC_BIT(black_threat_map, white_king);

	white_en_passant = -1;
	black_en_passant = -1;


	// Other player's turn
	if (whose_turn == white) whose_turn = black;
	else whose_turn = white;
}

bool Board::hasLegalMove(Color c) {
	for (int i = 0; i < 64; i++) {
		Piece* p = getPiece(i);
		if (!p) continue;
		if (p->getColor() == c) {
			if (p->legalMoves()->size() != 0) return true;
		}
	}
	return false;
}

bool Board::isInCheck(Color c) {
	return c == black && black_check || c == white && white_check;
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

bool Board::move(Piece* piece, glm::vec2 square) {
	if (!piece) return false;
	// Clear old spot
	glm::vec2 origin = piece->getPosition();
	board[(int)BIDX(glm::vec2(origin.x, origin.y))] = nullptr;

	// Move to new spot
	piece->place(square);
	board[(int)BIDX(glm::vec2(square.x, square.y))] = piece;


	// Extra move on castle
	if (instanceof<King>(piece) && abs(origin.x - square.x) == 2) {
		if (piece->getColor() == white) {
			if (square.x == 6) {
				board[(int)BIDX(glm::vec2(5, 0))] = board[(int)BIDX(glm::vec2(7, 0))];
				board[(int)BIDX(glm::vec2(7, 0))] = nullptr;
			}
			if (square.x == 2) {
				board[(int)BIDX(glm::vec2(3, 0))] = board[(int)BIDX(glm::vec2(0, 0))];
				board[(int)BIDX(glm::vec2(0, 0))] = nullptr;
			}
		}
		if (piece->getColor() == black) {
			if (square.x == 6) {
				board[(int)BIDX(glm::vec2(5, 7))] = board[(int)BIDX(glm::vec2(7, 7))];
				board[(int)BIDX(glm::vec2(7, 7))] = nullptr;
			}
			if (square.x == 2) {
				board[(int)BIDX(glm::vec2(3, 7))] = board[(int)BIDX(glm::vec2(0, 7))];
				board[(int)BIDX(glm::vec2(0, 7))] = nullptr;
			}
		}
	}

	// Check for promotion
	int promotion_sqr = (piece->getColor() == white) ? 7 : 0;
	if (instanceof<Pawn>(piece) && (square.y == promotion_sqr)) {
		promoting = square.x;
		return 1;
	}
	return 0;
}