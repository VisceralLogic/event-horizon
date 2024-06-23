#pragma once
#include <Windows.h>
#include <gl/GLU.h>

class Texture {
public:
	static void loadTexture(const char* file, GLuint* tex, int* width, int* height);
	//static void loadTextureFromGWorld(GWorldPtr offScreen, GLuint* tex);
};