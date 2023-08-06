#pragma once

#include <gl/glew.h>

class VBO {
public:
	GLuint ID;

	VBO(GLfloat* vertices, GLsizeiptr size);
	~VBO();
	void bind();
	void unbind();
};