#pragma once

#include "sphere.h"
#include "shader.h"

class Background {
	static int count;
	static int* x, * y, * z;
	static float* size;
	static float* brightness;
	static Sphere* sphere;
	static float distance;
	static GLShaderProgram* shader;

public:
	static void initialize();
	static void draw();
	static void generate(float density, size_t seed);
};