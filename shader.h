#pragma once
#include "gl/glew.h"

class Shader {
public:
	GLuint ID;

	Shader() {}
	Shader(const char*, const char*);
	~Shader();
	void activate();
	void deactivate();
};