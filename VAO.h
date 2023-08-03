#pragma once

#include "VBO.h"

class VAO {
public:
	GLuint ID;

	VAO();
	~VAO();
	void linkVBO(VBO, GLuint);
	void bind();
	void unbind();
};