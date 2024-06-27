#include "Texture.h"
#include "piece.h"
#include "string"
#include "util.h"
#include <Windows.h>


Texture::Texture(Piece* piece) {
	createGLTexture(&textureData);
	if (piece) addPieceTexture(piece);
}

Texture::~Texture() {
	glDeleteTextures(1, &textureData);
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, textureData);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getTexture() {
	return textureData;
}

void Texture::createGLTexture(GLuint* outputTexture) {
	// setup opengl texture object 
	glGenTextures(1, outputTexture);
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unbind();
}

void Texture::addPieceTexture(Piece* piece) {
	int width, height, nChannels;

	// Get texture from file through stb image
	std::string texture_path = "res/textures/";
	texture_path.append((piece->getColor() == black) ? "black_" : "white_");
	texture_path += piece->textboardSymbol();
	texture_path.append(".png");
	unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nChannels, 0);
	if (data) {
		bind();
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
	unbind();
}

void* Texture::getTextureData() {
	return (void*)(intptr_t)textureData;
}