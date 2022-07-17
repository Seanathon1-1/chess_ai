#pragma once

struct Piece {
protected:
	char board_rep = '0';	// how the piece is represented on the board
	uint8_t file;			// a-h files as 0-7
	uint8_t rank;			// 1-8 ranks as 0-7 (just subtract one, I know it's confusing)
	bool color;				// 0 for white, 1 for black
public:
	Piece(int f, int r, bool c) : file(f), rank(r), color(c) {}
	inline uint8_t getRank() { return rank; }
	inline uint8_t getFile() { return file; }
	inline char getBoardRep() { return board_rep; }
	inline bool getColor() { return color; }
};

struct Rook : Piece {
	Rook(int f, int r, int c) : Piece(f, r, c) { board_rep = 'R'; }
};

struct Pawn : Piece {
private:
	bool has_power = 1;
public:
	Pawn(int f, int r, int c) : Piece(f, r, c) { board_rep = '^'; }
};

struct Knight : Piece {
	Knight(int f, int r, int c) : Piece(f, r, c) { board_rep = 'N'; }
};

struct Bishop : Piece {
	Bishop(int f, int r, int c) : Piece(f, r, c) { board_rep = 'B'; }
};

struct Queen : Piece {
	Queen(int f, int r, int c) : Piece(f, r, c) { board_rep = 'Q'; }
};

struct King : Piece {
	King(int f, int r, int c) : Piece(f, r, c) { board_rep = 'K'; }
};