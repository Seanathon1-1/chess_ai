#pragma once

#include <gl/glew.h>

// Wrapper class for opengl Vertex Buffer Objects
class VBO {
public:
	GLuint ID;

	VBO(GLfloat* vertices, GLsizeiptr size);
	~VBO();
	void bind();
	void unbind();
};