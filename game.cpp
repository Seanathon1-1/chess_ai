#include "game.h"
#include "piece.h"
#include "imgui.h"

Game::Game(unsigned int fbo = 0) {
	board = new Board(fbo);
}

Game::Game(Game* base) {
	board = new Board(base->board);
}

Game::~Game() {
	delete board;
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
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !board->isHolding()) {
		ImVec2 wPos  = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos  = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file    = mPos.x / (wSize.x / 8);
		char rank    = 8 - mPos.y / (wSize.y / 8);
		Piece* p     = board->getPiece(BIDX(glm::vec2(file, rank)));
		if (p && p->getColor() == board->whoseTurn()) {
			board->grab(p);
		}
	} 
	else if (ImGui::IsMouseReleased(0) && board->isHolding()) {
		ImVec2 wPos = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file = mPos.x / (wSize.x / 8);
		char rank = 8 - mPos.y / (wSize.y / 8);
		Piece* targetPiece = board->getPiece(BIDX(glm::vec2(file, rank)));
		
		Piece* movedPiece = board->drop();
		if (movedPiece) {
			vec2s* legals = movedPiece->legalMoves(false);
			glm::vec2 attempt = { file,rank };
			if (std::find(legals->begin(), legals->end(), attempt) != legals->end()) {
				// TODO: move the piece
				board->makeLegalMove(movedPiece, attempt);
			}
			delete legals;
		}
	}
	ImGui::End();

	/*// Handle user entered moves
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
	}*/
	ImGui::End();
}