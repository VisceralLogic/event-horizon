#pragma once

#include <glad/glad.h>
#include <string>

using namespace std;

class Texture {
public:
	static void loadTexture(const string& file, GLuint* tex, int* width, int* height);
	static void loadTexture(const char* file, GLuint* tex, int* width, int* height);
	//static void loadTextureFromGWorld(GWorldPtr offScreen, GLuint* tex);
};