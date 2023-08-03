#include "EBO.h"

EBO::EBO(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	unbind();
}

EBO::~EBO() {
	glDeleteBuffers(1, &ID);
}

void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}