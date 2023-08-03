#include "shader.h"

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

Shader::Shader(const char* vertexFilepath, const char* fragmentFilepath) {
	std::string vertexString = get_file_contents(vertexFilepath);
	std::string fragmentString = get_file_contents(fragmentFilepath);

	const char* vertexSource = vertexString.c_str();
	const char* fragmentSource = fragmentString.c_str();

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

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