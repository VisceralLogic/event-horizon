#include "sphere.h"

#include <glad/glad.h>
#include <cmath>
#include <iostream>

using namespace std;

constexpr double M_PI = 3.14159265358979323846;

Sphere::Sphere(int slices, int stacks) {
	int numVertices = slices * (stacks - 1) + 2;

	float* vertices = new float[8 * numVertices];

	setVertex(vertices, 0, 0, 1, 0, 0, 1, 0, 0.5, 1);
	setVertex(vertices, numVertices-1, 0, -1, 0, 0, -1, 0, 0.5, 0);

	float phi, theta;
	int index = 1;
	for (int i = 1; i < stacks; i++) {
		phi = M_PI * i / stacks;
		for (int j = 0; j < slices; j++) {
			theta = 2 * M_PI * j / slices;
			setVertex(vertices, index++, sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta), sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta), (float)j / slices, 1 - (float)i / stacks);
		}	
	}

	// calculate indices
	numIndices = 6 * slices * (stacks - 1);
	unsigned int* indices = (unsigned int*)calloc(numIndices, sizeof(unsigned int));
	index = 0;
	for( int i = 1; i <= slices; i++ ) {
		indices[index++] = 0;
		indices[index++] = i;
		indices[index++] = ((i+slices) % slices) + 1;
	}
	for( int i = 0; i < stacks - 2; i++ ) {
		for( int j = 1; j <= slices; j++ ) {
			indices[index++] = j + slices * i;
			indices[index++] = j + slices + slices * i;
			indices[index++] = ((j + slices) % slices) + 1 + slices * i;		

			indices[index++] = ((j + slices) % slices) + 1 + slices * i;
			indices[index++] = j + slices + slices * i;
			indices[index++] = ((j + slices) % slices) + 1 + slices + slices * i;
		}
	}
	for( int i = 1; i <= slices; i++ ) {
		indices[index++] = numVertices - 1;
		indices[index++] = numVertices - (i+1);
		indices[index++] = numVertices - ((i + slices)%slices+2);
	}

	// create vao, vbo, and ebo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBufferData(GL_ARRAY_BUFFER, 8 * numVertices * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	delete[] vertices;
	delete[] indices;
}

Sphere::~Sphere() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Sphere::draw() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void Sphere::setVertex(float *vertices, int index, float x, float y, float z, float nx, float ny, float nz, float u, float v) {
	vertices[index * 8] = x;
	vertices[index * 8 + 1] = y;
	vertices[index * 8 + 2] = z;
	vertices[index * 8 + 3] = nx;
	vertices[index * 8 + 4] = ny;
	vertices[index * 8 + 5] = nz;
	vertices[index * 8 + 6] = u;
	vertices[index * 8 + 7] = v;
}