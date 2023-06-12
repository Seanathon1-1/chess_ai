#pragma once

#define ON_BOARD(v) ((0 <= (v) && (v) <= 7) ? 1 : 0)
#define BIDX(f,r) ((r)*8+(f))
#define XTRC_BIT(map, bit) (((map) >> (bit)) & 1ULL)

#define HORZ_LINE "|---|---|---|---|---|---|---|---|\n"

#define WIN_TITLE "Chess AI"
#define WIN_WIDTH  800
#define WIN_HEIGHT 600
#define NULL_UINT = 0xFFFFFFFF

enum PieceType { open, pawn, knight, bishop, rook, queen, king };
enum Color { black = -1, none, white };
