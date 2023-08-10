#include "VAO.h"

// Creates an opengl Vertex Array Object
VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

// Deletes the opengl object
VAO::~VAO() {
	glDeleteVertexArrays(1, &ID);
}

/*-------------------------------------------------------------------------------------------------------------*\
* VAO::linkAttribute(VBO*, GLuint, GLuint, GLenum, GLsizeiptr, void*)
* 
* Parameters: vbo - Pointer to the vbo containing the attribute
*             layout - Attribute index
*             numComponents - Number of components in the attribute
*             type - Primitive data type of the components
*             stride - Total length of data between vertices, in bytes
*             offset - Offset to the beginning of this attribute's data, in bytes as a void pointer
* Description: Links an attribute in 'vbo' to the VAO
\*-------------------------------------------------------------------------------------------------------------*/
void VAO::linkAttribute(VBO* vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	vbo->bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo->unbind();
}

// Binds the VAO
void VAO::bind() {
	glBindVertexArray(ID);
}

// Unbinds any VAO
void VAO::unbind() {
	glBindVertexArray(0);
}