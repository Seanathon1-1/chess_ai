#pragma once

#include <gl/glew.h>

class VBO {
public:
	GLuint ID;

	VBO(GLfloat*, GLsizeiptr);
	~VBO();
	void bind();
	void unbind();
};