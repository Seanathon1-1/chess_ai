#pragma once
#include "game.h"
#include "util.h"
#include "Texture.h"

class Piece : public std::enable_shared_from_this<Piece> {
protected:
	Color		m_color;
	uint8_t		m_position;
	Texture*    m_texture;
	bool		m_selected;

public:
	Piece(Color, uint8_t);
	~Piece();

	Color getColor() const { return m_color; }
	void select() { m_selected = true; }
	void deselect() { m_selected = false; }
	bool isSelected() const { return m_selected; }
	GLuint getTexture() { return m_texture->getTexture(); }
	uint8_t getPosition() const { return m_position; }
	void place(uint8_t pos) { m_position = pos; }
	void createTexture();
	
	virtual std::shared_ptr<Piece> copy() = 0;
	virtual void possibleMoves(std::vector<Move>*, std::shared_ptr<Board>, bool = false) = 0;
	virtual char textboardSymbol() = 0;
};

class Knight : public Piece {
public:
	using Piece::Piece;
	std::shared_ptr<Piece> copy() override;
	void possibleMoves(std::vector<Move>*, std::shared_ptr<Board>, bool) override;
	char textboardSymbol() override { return 'N'; }
};

class Bishop : public Piece {
public:
	using Piece::Piece;
	void possibleMoves(std::vector<Move>*, std::shared_ptr<Board>, bool) override;
	std::shared_ptr<Piece> copy() override;
	char textboardSymbol() override { return 'B'; }
};

class Rook : public Piece {
public:
	using Piece::Piece;
	std::shared_ptr<Piece> copy() override;
	void possibleMoves(std::vector<Move>*, std::shared_ptr<Board>, bool) override;
	char textboardSymbol() override { return 'R'; }
};

class Queen : public Piece {
public:
	using Piece::Piece;
	std::shared_ptr<Piece> copy() override;
	void possibleMoves(std::vector<Move>*, std::shared_ptr<Board>, bool) override;
	char textboardSymbol() override { return 'Q'; }
};

class King : public Piece {
public:
	using Piece::Piece;
	std::shared_ptr<Piece> copy() override;
	void possibleMoves(std::vector<Move>*, std::shared_ptr<Board>, bool) override;
	char textboardSymbol() override { return 'K'; }
};

class Pawn : public Piece {
	bool m_canDoubleMove = true;
public:
	using Piece::Piece;
	std::shared_ptr<Piece> copy() override;
	void possibleMoves(std::vector<Move>*, std::shared_ptr<Board>, bool) override;
	void losePower() { m_canDoubleMove = false; }
	char textboardSymbol() override { return 'P'; }
};