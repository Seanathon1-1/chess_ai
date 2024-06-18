#pragma once
#include "GL/glew.h"

class Piece;

class Texture {
	GLuint textureData;

	void createGLTexture(Piece* piece, GLuint* outputTexture);
public:
	Texture(Piece* piece);
	~Texture();
	GLuint getTexture();
	void* getTextureData();
};
