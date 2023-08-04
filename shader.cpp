#include "shader.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include <fstream>
#include <string>
#include <cerrno>

// Taken from insane coding
std::string get_file_contents(const char* filepath) {
	std::ifstream in(filepath, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw errno;
}

Shader::Shader(const char* vertexFilename, const char* fragmentFilename) {
    // read in the shader files
    std::string vertexSourceString = get_file_contents(vertexFilename);
    std::string fragmentSourceString = get_file_contents(fragmentFilename);
    const char* vertexSource = vertexSourceString.c_str();
    const char* fragmentSource = fragmentSourceString.c_str();

    // create vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

    // create fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

    // link shaders together
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

    // cleanup
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

void Shader::activate() {
	glUseProgram(ID);
}

void Shader::deactivate() {
	glUseProgram(0);
}