#include "square.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include <Windows.h>


// Colors for the squares of the boardsquare
#define LIGHT_SQUARE_COLOR glm::vec3(1.f) 
#define DARK_SQUARE_COLOR glm::vec3(.1f, 0.f, .2f)

// Offsets to the corners of the board squares
#define SQUARE_SIZE .25f
glm::vec3 square_top_right_offset = glm::vec3(SQUARE_SIZE, 0.f, 0.f);
glm::vec3 square_bottom_left_offset = glm::vec3(0.f, -1 * SQUARE_SIZE, 0.f);
glm::vec3 square_bottom_right_offset = glm::vec3(SQUARE_SIZE, -1 * SQUARE_SIZE, 0.f);

/*-------------------------------------------------------------------------------------------------------------*\
* Square::draw(Shader*)
*
* Parameters: shader - Pointer to shader used to draw our square.
* Description: Draws a solid colored square
\*-------------------------------------------------------------------------------------------------------------*/
void Square::draw(Shader* shader) {
	// Vertex data for this square
	glm::vec3 top_right_corner = top_left_corner + square_top_right_offset;
	glm::vec3 bottom_left_corner = top_left_corner + square_bottom_left_offset;
	glm::vec3 bottom_right_corner = top_left_corner + square_bottom_right_offset;
	glm::vec3 color = (is_dark) ? DARK_SQUARE_COLOR : LIGHT_SQUARE_COLOR;

	// Vertex and index buffer data
	float vertices[] = {
		top_left_corner.x, top_left_corner.y, top_left_corner.z, color.x, color.y, color.z,
		top_right_corner.x, top_right_corner.y, top_right_corner.z, color.x, color.y, color.z,
		bottom_left_corner.x, bottom_left_corner.y, bottom_left_corner.z, color.x, color.y, color.z,
		bottom_right_corner.x, bottom_right_corner.y, bottom_right_corner.z, color.x, color.y, color.z
	};
	unsigned int indices[] = { 0, 1, 2, 1, 2, 3 };

	// Create opengl objects
	VAO* vertex_array = new VAO();
	vertex_array->bind();
	VBO* vertex_buffer = new VBO(vertices, sizeof(vertices));
	EBO* element_buffer = new EBO(indices, sizeof(indices));

	// Link the position and color attributes
	vertex_array->linkAttribute(vertex_buffer, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	vertex_array->linkAttribute(vertex_buffer, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// Draw
	shader->activate();
	vertex_array->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Cleanup
	shader->deactivate();
	vertex_array->unbind();
	vertex_buffer->unbind();
	element_buffer->unbind();
	delete vertex_array;
	delete vertex_buffer;
	delete element_buffer;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Square::drawTexture(Shader*)
*
* Parameters: shader - Pointer to shader used to draw the piece texture within this square
* Description: Draws a chess piece over this square
\*-------------------------------------------------------------------------------------------------------------*/
void Square::drawTexture(Shader* shader) {
	// Vertex data for this square
	glm::vec3 top_right_corner = top_left_corner + square_top_right_offset;
	glm::vec3 bottom_left_corner = top_left_corner + square_bottom_left_offset;
	glm::vec3 bottom_right_corner = top_left_corner + square_bottom_right_offset;

	// Vertex and index buffer data 
	GLfloat vertices[] = {
		top_left_corner.x, top_left_corner.y, top_left_corner.z - .1f, 0.f, 1.f,
		top_right_corner.x, top_right_corner.y, top_right_corner.z - .1f, 1.f, 1.f,
		bottom_left_corner.x, bottom_left_corner.y, bottom_left_corner.z - .1f, 0.f, 0.f,
		bottom_right_corner.x, bottom_right_corner.y, bottom_right_corner.z - .1f, 1.f, 0.f
	};
	GLuint indices[] = { 0, 1, 2, 1, 2, 3 };

	// Create opengl elements
	VAO* vertex_array = new VAO();
	vertex_array->bind();
	VBO* vertex_buffer = new VBO(vertices, sizeof(vertices));
	EBO* element_buffer = new EBO(indices, sizeof(indices));

	// Link position and texture coordinate attributes
	vertex_array->linkAttribute(vertex_buffer, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	vertex_array->linkAttribute(vertex_buffer, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// setup opengl texture object 
	int width, height, nChannels;
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Get texture from file through stb image
	std::string texture_path = "res/textures/";
	texture_path.append((piece->getColor() == black) ? "black_" : "white_");
	texture_path += piece->textboardSymbol();
	texture_path.append(".png");
	unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::cerr << "Fail!: " << texture_path << "\n";
		std::cerr << "Working Directory: " << buffer << std::endl;
	}
	stbi_image_free(data);


	// Draw the piece
	shader->activate();
	glEnable(GL_BLEND);
	glUniform1i(glGetUniformLocation(shader->ID, "ourTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	vertex_array->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Cleanup
	shader->deactivate();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	vertex_array->unbind();
	vertex_buffer->unbind();
	element_buffer->unbind();
	glDeleteTextures(1, &texture);
	delete vertex_array;
	delete vertex_buffer;
	delete element_buffer;
}