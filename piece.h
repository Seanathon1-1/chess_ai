#pragma once

class Piece {
public:
	char board_rep = '0';	// how the piece is represented on the board
	uint8_t file;			// a-h files as 0-7
	uint8_t rank;			// 1-8 ranks as 0-7 (just subtract one, I know it's confusing)
	int color;				// 1 for white, -1 for black
public:
	Piece(int f, int r, int c) : file(f), rank(r), color(c) {}
	inline uint8_t getRank() { return rank; }
	inline uint8_t getFile() { return file; }
	inline char getBoardRep() { return board_rep; }
	inline int getColor() { return color; }
	virtual void moves() {} 
};

class Rook : public Piece {
public:
	Rook(int f, int r, int c) : Piece(f, r, c) { board_rep = 'R'; }
};

class Pawn : public Piece {
public:
	bool has_power = 1;
	Pawn(int f, int r, int c) : Piece(f, r, c) { board_rep = '^'; }
};

class Knight : public Piece {
public:
	Knight(int f, int r, int c) : Piece(f, r, c) { board_rep = 'N'; }
};

class Bishop : public Piece {
public:
	Bishop(int f, int r, int c) : Piece(f, r, c) { board_rep = 'B'; }
};

class Queen : public Piece {
public:
	Queen(int f, int r, int c) : Piece(f, r, c) { board_rep = 'Q'; }
};

class King : public Piece {
public:
	King(int f, int r, int c) : Piece(f, r, c) { board_rep = 'K'; }
};