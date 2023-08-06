#pragma once

#include <gl/glew.h>

class EBO {
public:
	GLuint ID;

	EBO(GLuint* indices, GLsizeiptr size);
	~EBO();
	void bind();
	void unbind();
};