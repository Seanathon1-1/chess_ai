#pragma once
#include "piece.h"

class Square {
	glm::vec3 top_left_corner;
	bool is_dark;
	Piece* piece;

public:
	Square(glm::vec3 tlc, bool dark, Piece* p) : top_left_corner(tlc), is_dark(dark), piece(p) {}
	void draw(Shader*);
	void drawTexture(Shader*);
};