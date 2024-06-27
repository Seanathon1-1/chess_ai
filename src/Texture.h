#pragma once
#include "GL/glew.h"

class Piece;

class Texture {
	GLuint textureData;

	void createGLTexture(GLuint* outputTexture);
	void addPieceTexture(Piece* piece);
public:
	Texture(Piece* piece);
	~Texture();
	void bind();
	void unbind();
	GLuint getTexture();
	void* getTextureData();
};
