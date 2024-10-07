#pragma once

#include "sphere.h"

class Background {
	static int count;
	static int* x, * y, * z;
	static Sphere* sphere;
	static float distance;

public:
	static void initialize();
	static void draw();
	static void generate(float density);
};