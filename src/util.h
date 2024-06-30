#pragma once

#include <vector>
#include <stdint.h>
#include <iostream>
#include <glm/glm.hpp>
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"

#define ON_BOARD(s) ((0 <= (s) && (s) <= 7) ? 1 : 0)

constexpr const char* HORZ_LINE = "|---|---|---|---|---|---|---|---|\n";

constexpr const char* WIN_TITLE = "Chess AI";
constexpr uint16_t WIN_WIDTH = 800;
constexpr uint16_t WIN_HEIGHT = 600;
constexpr uint32_t NULL_UINT = 0xFFFFFFFF;

enum PieceType { open, pawn, knight, bishop, rook, queen, king };
enum Color { black = -1, none, white };

void print_vec3(glm::vec3, std::ostream& os = std::cerr);

template<typename Base, typename T>
inline bool instanceof(const T* ptr) {
	return dynamic_cast<const Base*>(ptr) != nullptr;
}

typedef std::vector<glm::ivec2> vec2s;
typedef std::vector<glm::vec3> vec3s;
typedef std::vector<glm::vec4> vec4s;
typedef std::vector<unsigned int> uints;