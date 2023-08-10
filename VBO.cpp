# include "VBO.h"

/*-------------------------------------------------------------------------------------------------------------*\
* VBO::VBO(GLfloat*, GLsizeiptr)
* 
* Parameters: vertices - Pointer to contiguous data array of vertex information for the VBO, in floats
*             size - Size of the 'vertices' array, in bytes
\*-------------------------------------------------------------------------------------------------------------*/
VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

// Deletes the opengl object for this VBO
VBO::~VBO() {
	glDeleteBuffers(1, &ID);
}

// Binds the VBO
void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the active VBO
void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}