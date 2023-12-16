#pragma once
#include "gl/glew.h"

// Wrapper for opengl shaders
class Shader {
public:
	GLuint ID;
	Shader(const char*, const char*);
	~Shader();
	void activate();
	void deactivate();
};