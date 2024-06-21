#pragma once

#include "util.h"
#include "board.h"
#include "Texture.h"

class Piece {
protected:
	Color		m_color;
	uint8_t		m_position;
	Board*		m_board;
	Texture*    m_texture;
	bool		m_selected;

public:
	Piece(Color, uint8_t, Board*);
	~Piece();

	Color getColor() { return m_color; }
	void select() { m_selected = true; }
	void deselect() { m_selected = false; }
	bool isSelected() { return m_selected; }
	GLuint getTexture() { return m_texture->getTexture(); }
	uint8_t getPosition() { return m_position; }
	void place(uint8_t pos) { m_position = pos; }
	void createTexture();
	bool check4check(uint8_t, bool = false);
	
	virtual Piece* copy(Board*) = 0;
	virtual std::vector<uint8_t>* legalMoves(bool = false) = 0;
	virtual char textboardSymbol() = 0;
};

class Knight : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	std::vector<uint8_t>* legalMoves(bool) override;
	char textboardSymbol() override { return 'N'; }
};

class Bishop : public Piece {
public:
	using Piece::Piece;
	std::vector<uint8_t>* legalMoves(bool) override;
	Piece* copy(Board*) override;
	char textboardSymbol() override { return 'B'; }
};

class Rook : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	std::vector<uint8_t>* legalMoves(bool) override;
	char textboardSymbol() override { return 'R'; }
};

class Queen : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	std::vector<uint8_t>* legalMoves(bool) override;
	char textboardSymbol() override { return 'Q'; }
};

class King : public Piece {
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	std::vector<uint8_t>* legalMoves(bool) override;
	char textboardSymbol() override { return 'K'; }
};

class Pawn : public Piece {
	bool m_canDoubleMove = true;
public:
	using Piece::Piece;
	Piece* copy(Board*) override;
	std::vector<uint8_t>* legalMoves(bool) override;
	void losePower() { m_canDoubleMove = false; }
	char textboardSymbol() override { return 'P'; }
};