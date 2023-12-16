#include "EBO.h"

/*-------------------------------------------------------------------------------------------------------------*\
* EBO::EBO(GLuint*, GLsizeiptr)
* 
* Parameters: indices - Pointer to contiguous data array of indices for the EBO, in uints
*             size - Size of the 'indices' array, in bytes
\*-------------------------------------------------------------------------------------------------------------*/
EBO::EBO(GLuint* indices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

// Deletes the EBO from opengl
EBO::~EBO() {
	glDeleteBuffers(1, &ID);
}

// Binds the EBO
void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds any EBO
void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}