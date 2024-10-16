#pragma once

#include <glad/glad.h>

class Sphere {
	GLuint vao, vbo, ebo;
	int numIndices;

	void setVertex(float *vertices, int index, float x, float y, float z, float u, float v);

public:
	Sphere(int slices, int stacks);
	~Sphere();

	void draw();
};