#pragma once

#include <gl/glew.h>

class EBO {
public:
	GLuint ID;

	EBO(GLfloat*, GLsizeiptr);
	~EBO();
	void bind();
	void unbind();
};