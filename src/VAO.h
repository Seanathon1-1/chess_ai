#pragma once


#include "VBO.h"

// Wrapper class for opengl Vertex Array Objects
class VAO {
public:
	GLuint ID;

	VAO();
	~VAO();
	void linkAttribute(VBO* vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void bind();
	void unbind();
};