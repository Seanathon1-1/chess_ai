#pragma once
#include "util.h"
#include "game.h"

class Piece {
protected:
	Color     m_color;
	glm::vec2 m_position;
	Board*    m_board;
	bool      m_selected;

public:
	Piece(Color, glm::vec2, Board*);
	Color getColor() { return m_color; }
	bool isSelected() { return m_selected; }
	
	virtual std::vector<glm::vec2>& legalMoves() = 0;
	virtual char textboardSymbol() = 0;
};

class Knight : private Piece {
	std::vector<glm::vec2>& legalMoves() override;
	char textboardSymbol() override { return 'N'; }
};

class Bishop : Piece {
	std::vector<glm::vec2>& legalMoves() override;
	char textboardSymbol() override { return 'B'; }
};

class Rook : Piece {
	std::vector<glm::vec2>& legalMoves() override;
	char textboardSymbol() override { return 'R'; }
};

class Queen : Piece {
	std::vector<glm::vec2>& legalMoves() override;
	char textboardSymbol() override { return 'Q'; }
};

class King : Piece {
	std::vector<glm::vec2>& legalMoves() override;
	char textboardSymbol() override { return 'K'; }
};

class Pawn : Piece {
	std::vector<glm::vec2>& legalMoves() override;
	char textboardSymbol() override { return 'P'; }
};