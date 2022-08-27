#pragma once

#define ON_BOARD(v) ((0 <= (v) && (v) <= 7) ? 1 : 0)
#define BIDX(f,r) ((r)*8+(f))
#define XTRC_BIT(map, bit) (((map) >> (bit)) & 1ULL)

#define HORZ_LINE "|---|---|---|---|---|---|---|---|\n"