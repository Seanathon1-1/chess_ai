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
	void select() { m_selected = true; }
	void deselect() { m_selected = false; }
	bool isSelected() { return m_selected; }
	
	virtual std::vector<glm::vec2>& legalMoves(bool) = 0;
	virtual char textboardSymbol() = 0;
};

class Knight : public Piece {
public:
	using Piece::Piece;
	std::vector<glm::vec2>& legalMoves(bool) override;
	char textboardSymbol() override { return 'N'; }
};

class Bishop : public Piece {
public:
	using Piece::Piece;
	std::vector<glm::vec2>& legalMoves(bool) override;
	char textboardSymbol() override { return 'B'; }
};

class Rook : public Piece {
public:
	using Piece::Piece;
	std::vector<glm::vec2>& legalMoves(bool) override;
	char textboardSymbol() override { return 'R'; }
};

class Queen : public Piece {
public:
	using Piece::Piece;
	std::vector<glm::vec2>& legalMoves(bool) override;
	char textboardSymbol() override { return 'Q'; }
};

class King : public Piece {
public:
	using Piece::Piece;
	std::vector<glm::vec2>& legalMoves(bool) override;
	char textboardSymbol() override { return 'K'; }
};

class Pawn : public Piece {
public:
	using Piece::Piece;
	std::vector<glm::vec2>& legalMoves(bool) override;
	char textboardSymbol() override { return 'P'; }
};