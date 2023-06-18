#pragma once

#include "util.h"

struct Triangle {
	Triangle(vec3s, vec3s);
	~Triangle();

	void draw(const glm::mat4, GLuint);
	
private:
	// Graphical Components
	GLuint vertex_array_object;
	GLuint vertex_buffer_object_positions;
	GLuint vertex_buffer_object_normals;
	GLuint element_buffer_object;	
	glm::mat4 projection;
	glm::vec3 color;
	vec3s positions;
	vec3s normals;
	uints indices;
};