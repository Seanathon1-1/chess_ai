#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "gl/glew.h"
#include "GLFW/glfw3.h"


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


typedef std::vector<glm::vec3> vec3s;
typedef std::vector<glm::vec4> vec4s;
typedef std::vector<unsigned int> uints;

