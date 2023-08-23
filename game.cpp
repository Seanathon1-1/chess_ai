#include "game.h"
#include "imgui.h"

/*-------------------------------------------------------------------------------------------------------------*\
* Game::Game(GLuint)
* 
* Parameters: fbo - framebuffer object that this game will be drawn onto
* Description: Creates a new game in the starting poistion and creates a Board object for it's representation
\*-------------------------------------------------------------------------------------------------------------*/
Game::Game(unsigned int fbo = 0) {

}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::Game(Game*)
* 
* Parameters: base - Pointer to a Game instance that we want to copy
* Description: Copies a position into a new Game instance. Mostly used to tell how a theoretical move will go
\*-------------------------------------------------------------------------------------------------------------*/
Game::Game(Game* base) {
	board = new Board(base->board);
}

// I don't think I need a big comment for this destructor.
Game::~Game() {
	if (board) delete board;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::makeUserMove(std::string)
* 
* Parameters: move - String representation of the move we want to make. Must be of form a1b2.
* Description: Checks that a move is legal then makes the move and checks if the game is over.
\*-------------------------------------------------------------------------------------------------------------*/
void Game::makeUserMove(std::string move) {
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
	int select_idx = BIDX(sf, sr);
	Piece* selected = board->getPiece(select_idx);
	if (!selected) {
		std::cout << "No piece selected to move.\n";
		return;
	} 

	// Check that it is the correct player's turn
	if (selected->getColor()!= board->whoseTurn()) {
		std::cout << "Can't move other player's piece.\n";
		return;
	}

	// Check if the destination is a legal move
	int dest_idx = BIDX(df, dr);
	std::vector<int> moves_possible;
	legalPieceMoves(&moves_possible, selected, sf, sr);
	bool move_exists = 0;
	for (int move : moves_possible) if (move == dest_idx) { move_exists = 1; break; }
	if (!move_exists) {
		std::cout << "Move is not legal.\n";
		return;
	}

	// Makes the move and changes whose turn it is
	makeLegalMove(*selected, select_idx, dest_idx);

	// Look for checkmate/stalemate
	std::vector<int> nextPlayerMoves;
	allLegalMoves(&nextPlayerMoves, whose_turn);
	if (nextPlayerMoves.size() == 0) {
		// Checkmate
		if (whose_turn == white && white_check) {
			std::cout << "0-1" << std::endl;
			return;
		}
		else if (whose_turn == black && black_check) {
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
void Game::makeLegalMove(Piece* p, int src, int dest) {
	wait_for_promote |= board->makeMove(p, src, dest);

	// Look for check
	updateThreatMaps();
	black_check = XTRC_BIT(white_threat_map, black_king);
	white_check = XTRC_BIT(black_threat_map, white_king);

	// Enforce castling restrictions
	if (p.kind == king) {
		if (p.color == white) {
			white_short_castle = 0;
			white_long_castle = 0;
		}
		if (p.color == black) {
			black_short_castle = 0;
			black_long_castle = 0;
		}
	}
	if (p.kind == rook) {
		if (p.color == white) {
			if (src % 8 == 7 && src / 8 == 0) white_short_castle = 0;
			if (src % 8 == 0 && src / 8 == 0) white_long_castle = 0;
		}
		if (p.color == black) {
			if (src % 8 == 7 && src / 8 == 7) black_short_castle = 0;
			if (src % 8 == 0 && src / 8 == 7) black_long_castle = 0;
		}
	}

	// Other player's turn
	if (whose_turn == white) whose_turn = black;
	else whose_turn = white;

	// Check if en passant is available for next move
	white_en_passant = -1;
	black_en_passant = -1;
	if (p.kind == pawn) {
		int dist = abs((dest / 8) - (src / 8));
		int pawn_file = src % 8;
		if (dist == 2) (p.color == white) ? white_en_passant = pawn_file : black_en_passant = pawn_file;
	}

	if (p.kind == king) (p.color == white) ? white_king = dest : black_king = dest;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::allLegalMoves(std::vector<int>*, Color)
* 
* Parameters: moves - Pointer to a vector where legal moves will be appended
*             c - Color of the piece we want to look at.
* Description: Cycles through all of the pieces of a certain color and appends the legal moves to the vector
*              passed in. 'moves' is assumed to be empty
\*-------------------------------------------------------------------------------------------------------------*/
void Game::allLegalMoves(std::vector<int>* moves, Color c) {
	for (int i = 0; i < 64; i++) {
		if (board->getPiece(i)->color == c) {
			legalPieceMoves(moves, *board->getPiece(i), i % 8, i / 8);
		}
		if (moves->size() > 0) return;
	}
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::legalPieceMoves(std::vector<int>*, Piece, int, int)
* 
* Parameters: moves - Pointer to vector where legal moves will be appended
*             p - Which piece we are checking the moves for
*             file - File where the piece is located (numbered 0-7)
*             rank - Rank where the piece is located (numbered 0-7)
* Description: Appends the 'moves' vector with the moves that Piece 'p' can move to
\*-------------------------------------------------------------------------------------------------------------*/
void Game::legalPieceMoves(std::vector<int>* moves, Piece p, int file, int rank) {
	int square; int f;

	std::vector<int> possible_moves;

	if (p.kind == pawn) {
		int home_rank = (p.color == white) ? 1 : 6;
		int passant_rank = (p.color == white) ? 4 : 3;

		// Normal move
		square = BIDX(file, rank + (1 * p.color));
		if (board->getPiece(square)->kind == open) {
			possible_moves.push_back(square);
		}
		// Pawn power
		square = BIDX(file, rank + (2 * p.color));
		if (rank == home_rank && board->getPiece(square)->kind == open) {
			possible_moves.push_back(square);
		}

		// Captures
		pawnSights(moves, file, rank, p.color);

		// En passant
		if (rank == passant_rank) {
			f = (p.color == white) ? black_en_passant : white_en_passant;
			if (f == file - 1) possible_moves.push_back(BIDX(f, rank + (1 * p.color)));
			if (f == file + 1) possible_moves.push_back(BIDX(f, rank + (1 * p.color)));
		}
	}

	if (p.kind == knight) {
		knightSights(&possible_moves, file, rank, p.color);
	}

	if (p.kind == bishop || p.kind == queen) {
		bishopSights(&possible_moves, file, rank, p.color);
	}

	if (p.kind == rook || p.kind == queen) {
		rookSights(&possible_moves, file, rank, p.color);
	}

	if (p.kind == king) {
		kingSights(&possible_moves, file, rank, p.color);
		
	}

	// Look to see if move leaves player's king in check
	int source = BIDX(file, rank);
	for (auto iter = possible_moves.begin(); iter != possible_moves.end(); iter++) {
		Game* test_move = new Game(this);
		test_move->makeLegalMove(p, source, *iter);

		bool white_self_check = p.color == white && test_move->white_check;
		bool black_self_check = p.color == black && test_move->black_check;
		if (!white_self_check && !black_self_check) moves->push_back(*iter);
		delete test_move;
	}
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::pawnSights(std::vector<int>*, int, int, Color, bool?)
* 
* Parameters: moves - Pointer to vector where legal moves will be appended
*             p - Which piece we are checking the moves for
*             file - File where the piece is located (numbered 0-7)
*             rank - Rank where the piece is located (numbered 0-7)
*             threat - Optional, defaults to false. Are we checking threat maps or not?
* Description: Looks at the possible moves for a pawn and appends them to 'moves'
\*-------------------------------------------------------------------------------------------------------------*/
void Game::pawnSights(std::vector<int>* moves, int file, int rank, Color c, bool threat) {
	int square;
	square = BIDX(file - 1, rank + (1 * c));
	if (file != 0 && (board->getPiece(square)->color == (c * -1) || threat)) moves->push_back(square);
	square = BIDX(file + 1, rank + (1 * c));
	if (file != 7 && (board->getPiece(square)->color == (c * -1) || threat)) moves->push_back(square);
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::knightSights(std::vector<int>*, int, int, Color, bool?)
*
* Parameters: moves - Pointer to vector where legal moves will be appended
*             p - Which piece we are checking the moves for
*             file - File where the piece is located (numbered 0-7)
*             rank - Rank where the piece is located (numbered 0-7)
*             threat - Optional, defaults to false. Are we checking threat maps or not?
* Description: Looks at the possible moves for a knight and appends them to 'moves'
\*-------------------------------------------------------------------------------------------------------------*/
void Game::knightSights(std::vector<int>* moves, int file, int rank, Color c, bool threat) {
	
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::bishopSights(std::vector<int>*, int, int, Color, bool?)
*
* Parameters: moves - Pointer to vector where legal moves will be appended
*             p - Which piece we are checking the moves for
*             file - File where the piece is located (numbered 0-7)
*             rank - Rank where the piece is located (numbered 0-7)
*             threat - Optional, defaults to false. Are we checking threat maps or not?
* Description: Looks at the possible moves for a bishop and appends them to 'moves'
\*-------------------------------------------------------------------------------------------------------------*/
void Game::bishopSights(std::vector<int>* moves, int file, int rank, Color c, bool threat) {
	
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::rookSights(std::vector<int>*, int, int, Color, bool?)
*
* Parameters: moves - Pointer to vector where legal moves will be appended
*             p - Which piece we are checking the moves for
*             file - File where the piece is located (numbered 0-7)
*             rank - Rank where the piece is located (numbered 0-7)
*             threat - Optional, defaults to false. Are we checking threat maps or not?
* Description: Looks at the possible moves for a rook and appends them to 'moves'
\*-------------------------------------------------------------------------------------------------------------*/
void Game::rookSights(std::vector<int>* moves, int file, int rank, Color c, bool threat) {
	
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::kingSights(std::vector<int>*, int, int, Color, bool?)
*
* Parameters: moves - Pointer to vector where legal moves will be appended
*             p - Which piece we are checking the moves for
*             file - File where the piece is located (numbered 0-7)
*             rank - Rank where the piece is located (numbered 0-7)
*             threat - Optional, defaults to false. Are we checking threat maps or not?
* Description: Looks at the possible moves for a king and appends them to 'moves'
\*-------------------------------------------------------------------------------------------------------------*/
void Game::kingSights(std::vector<int>* moves, int file, int rank, Color c, bool threat) {
	
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
void Game::updateThreatMaps() {
	white_threat_map = 0ULL;
	black_threat_map = 0ULL;

	Piece* piece;
	std::vector<int> squares;
	uint64_t* threat_map;
	int f; int r;
	for (int i = 0; i < 64; i++) {
		squares.clear();
		piece = board->getPiece(i);
		if (piece->kind == open) continue;
		threat_map = (piece->color == white) ? &white_threat_map : &black_threat_map;

		// Find squares covered by this piece
		f = i % 8; r = i / 8;
		if (piece->kind == pawn) pawnSights(&squares, f, r, piece->color, true);
		if (piece->kind == knight) knightSights(&squares, f, r, piece->color, true);
		if (piece->kind == bishop || piece->kind == queen) bishopSights(&squares, f, r, piece->color, true);
		if (piece->kind == rook || piece->kind == queen) rookSights(&squares, f, r, piece->color, true);
		if (piece->kind == king) kingSights(&squares, f, r, piece->color, true);

		for (int sqr : squares) {
			set_bit(threat_map, sqr);
		}
 	}
}

/*-------------------------------------------------------------------------------------------------------------*\
* Game::render()
* 
* Description: Sets up the ImGUI context of our game, handles user entered moves, and handles pawn promotion
\*-------------------------------------------------------------------------------------------------------------*/
void Game::render() {
	ImGui::SetNextWindowPos(ImVec2(0, 0)); 
	ImGui::Begin("Play window", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	board->render();

	ImGui::Begin("Gameview");
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !selected) {
		ImVec2 wPos  = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos  = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file    = mPos.x / (wSize.x / 8);
		char rank    = 8 - mPos.y / (wSize.y / 8);
		Piece* p     = board->getPiece(BIDX(file, rank));
		if (p && p->getColor() == board->whoseTurn()) {
			selected = p;
			p->select();
		}
	}
	else if (ImGui::IsMouseReleased(0) && selected) {
		ImVec2 wPos = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file = mPos.x / (wSize.x / 8);
		char rank = 8 - mPos.y / (wSize.y / 8);
		Piece* p = board->getPiece(BIDX(file, rank));
		
		selected->selected = false;
		selected = nullptr;
	}
	ImGui::End();

	// Handle user entered moves
	char move[16] = "";
	bool move_entered = ImGui::InputText("Make Move", move, 16, ImGuiInputTextFlags_EnterReturnsTrue);

	// Block moves until promotion has been dealt with
	if (move_entered && !wait_for_promote) makeUserMove(move);
	if (wait_for_promote) {
		PieceType promote_to = open;
		if (ImGui::Button("Queen")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
		if (ImGui::Button("Knight")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
		if (ImGui::Button("Rook")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
		if (ImGui::Button("Bishop")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
	}
	ImGui::End();
}