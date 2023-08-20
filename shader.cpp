#include "shader.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include <fstream>
#include <string>
#include <cerrno>

// Taken from insane coding: reads in a file as a string
std::string get_file_contents(const char* filepath) {
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
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

/*-------------------------------------------------------------------------------------------------------------*\
* Shader::Shader(const char*, const char*)
* 
* Parameters: vertexFilepath - Filepath to the vertex shader
*             fragmentFilepath - FIlepath to the fragment shader
* Description: Creates a new opengl shader program with the given vertex and fragment shaders
\*-------------------------------------------------------------------------------------------------------------*/
Shader::Shader(const char* vertexFilepath, const char* fragmentFilepath) {
    // Read in the shader files
    std::string vertexSourceString = get_file_contents(vertexFilepath);
    std::string fragmentSourceString = get_file_contents(fragmentFilepath);
    const char* vertexSource = vertexSourceString.c_str();
    const char* fragmentSource = fragmentSourceString.c_str();

    // Create vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

    // Create fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

    // Link shaders together
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

    // Cleanup
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// Deletes the shader from opengl
Shader::~Shader() {
	glDeleteProgram(ID);
}

// Turns on this shader for use in drawing
void Shader::activate() {
	glUseProgram(ID);
}

// Turns off any shader currently active
void Shader::deactivate() {
	glUseProgram(0);
}