#include "game.h"
#include "piece.h"
#include "imgui.h"
#include "Texture.h"

Game::Game(unsigned int fbo = 0) {
	board = new Board(fbo, this);
}

Game::Game(Game* base) {
	board = new Board(base->board);
}

Game::~Game() {
	delete board;
}

void Game::createPromotionTextures() {
	queenPromotion = new Texture(&Queen(board->whoseTurn(), { 0, 0 }, 0));
	rookPromotion = new Texture(&Rook(board->whoseTurn(), { 0, 0 }, 0));
	knightPromotion = new Texture(&Knight(board->whoseTurn(), { 0, 0 }, 0));
	bishopPromotion = new Texture(&Bishop(board->whoseTurn(), { 0, 0 }, 0));
}

void Game::deletePromotionTextures() {
	delete queenPromotion;
	delete rookPromotion;
	delete knightPromotion;
	delete bishopPromotion;
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

	if (board->isWaitingOnPromotion()) {
		ImGui::Begin("Promotion Selection", 0, ImGuiWindowFlags_NoTitleBar);

		if (ImGui::ImageButton("Queen", queenPromotion->getTextureData(), ImVec2(70, 70))) {
			board->promote<Queen>();
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("Rook", rookPromotion->getTextureData(), ImVec2(70, 70))) {
			board->promote<Rook>();
		}

		if (ImGui::ImageButton("Knight", knightPromotion->getTextureData(), ImVec2(70, 70))) {
			board->promote<Knight>();
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("Bishop", bishopPromotion->getTextureData(), ImVec2(70, 70))) {
			board->promote<Bishop>();
		}


		ImGui::End();
	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !board->isHolding()) {
		ImVec2 wPos  = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos  = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file    = mPos.x / (wSize.x / 8);
		char rank    = 8 - mPos.y / (wSize.y / 8);
		Piece* p     = board->getPiece(glm::ivec2(file, rank));
		if (p && p->getColor() == board->whoseTurn() && !board->isWaitingOnPromotion()) {
			board->grab(p);
		}
	} 
	else if (ImGui::IsMouseReleased(0) && board->isHolding()) {
		ImVec2 wPos			= ImGui::GetWindowPos();
		ImVec2 wSize		= ImGui::GetWindowSize();
		ImVec2 mPos			= { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file			= mPos.x / (wSize.x / 8);
		char rank			= 8 - mPos.y / (wSize.y / 8);
		Piece* targetPiece	= board->getPiece(glm::ivec2(file, rank));
		Piece* movedPiece	= board->drop();
		if (movedPiece) {
			vec2s* legals = movedPiece->legalMoves();
			glm::ivec2 attempt = { file,rank };
			if (std::find(legals->begin(), legals->end(), attempt) != legals->end()) {
				board->makeLegalMove(movedPiece, attempt);
			}
			delete legals;
		}
	}
	ImGui::End();


	ImGui::End();
}


