#pragma once
#include "piece.h"

class Square {
	glm::vec3 top_left_corner;
	bool isLight;
	std::shared_ptr<Piece> piece;

public:
	Square(glm::vec3 tlc, bool light, std::shared_ptr<Piece> p) : top_left_corner(tlc), isLight(light), piece(p) {}
	void draw(Shader*);
	void drawTexture(Shader*);
};