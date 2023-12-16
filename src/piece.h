#pragma once

#include "util.h"
#include "board.h"

class Piece {
protected:
	Color     m_color;
	glm::ivec2 m_position;
	Board*    m_board;
	bool      m_selected;

	bool check4check(glm::ivec2);
public:
	Piece(Color, glm::ivec2, Board*);

	Color getColor() { return m_color; }
	void select() { m_selected = true; }
	void deselect() { m_selected = false; }
	bool isSelected() { return m_selected; }
	glm::ivec2 getPosition() { return m_position; }
	void place(glm::ivec2 pos) { m_position = pos; }
	
	virtual Piece* copy(Board*) = 0;
	virtual vec2s* legalMoves(bool = false) = 0;
	virtual char textboardSymbol() = 0;
};

class Knight : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	vec2s* legalMoves(bool) override;
	char textboardSymbol() override { return 'N'; }
};

class Bishop : public Piece {
public:
	using Piece::Piece;
	vec2s* legalMoves(bool) override;
	Piece* copy(Board*) override;
	char textboardSymbol() override { return 'B'; }
};

class Rook : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	vec2s* legalMoves(bool) override;
	char textboardSymbol() override { return 'R'; }
};

class Queen : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	vec2s* legalMoves(bool) override;
	char textboardSymbol() override { return 'Q'; }
};

class King : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	vec2s* legalMoves(bool) override;
	char textboardSymbol() override { return 'K'; }
};

class Pawn : public Piece {
	bool m_canDoubleMove = true;
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	vec2s* legalMoves(bool) override;
	void losePower() { m_canDoubleMove = false; }
	char textboardSymbol() override { return 'P'; }
};