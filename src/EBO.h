#pragma once

#include <GL/glew.h>

// Wrapper class for opengl Element Buffer Objects
class EBO {
public:
	GLuint ID;

	EBO(GLuint* indices, GLsizeiptr size);
	~EBO();
	void bind();
	void unbind();
};