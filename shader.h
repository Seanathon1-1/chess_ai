#pragma once

#include <gl/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char*);

class Shader {
public:
	GLuint ID;
	Shader(const char*, const char*);
	~Shader();

	void activate();
	void deactivate();
};