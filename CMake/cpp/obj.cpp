#include "obj.h"
#include <iostream>

OBJ::~OBJ() {
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void OBJ::generateBuffers(){
	// fill in vertex data
	triangles = 0;
	vector<float> vertexData;

	for (int j = 0; j < faces.size(); j++) {
		vector<shared_ptr<Coord>> face = faces[j];
		shared_ptr<Coord> a = face[0];
		for (int k = 1; k < face.size() - 1; k++) {
			shared_ptr<Coord> b = face[k];
			shared_ptr<Coord> c = face[k + 1];
			Coord normal = normalize(vertices[a->x - 1], vertices[b->x - 1], vertices[c->x - 1]);

			float v1[] = { vertices[a->x - 1]->x, vertices[a->x - 1]->y, vertices[a->x - 1]->z, smooth ? normal.x : normals[a->z - 1]->x, smooth ? normal.y : normals[a->z - 1]->y, smooth ? normal.z : normals[a->z - 1]->z, a->y ? uvCoords[a->y - 1]->x : 0.002f, a->y ? uvCoords[a->y - 1]->y : 0.002f };
			float v2[] = { vertices[b->x - 1]->x, vertices[b->x - 1]->y, vertices[b->x - 1]->z, smooth ? normal.x : normals[b->z - 1]->x, smooth ? normal.y : normals[b->z - 1]->y, smooth ? normal.z : normals[b->z - 1]->z, b->y ? uvCoords[b->y - 1]->x : 0.002f, b->y ? uvCoords[b->y - 1]->y : 0.002f };
			float v3[] = { vertices[c->x - 1]->x, vertices[c->x - 1]->y, vertices[c->x - 1]->z, smooth ? normal.x : normals[c->z - 1]->x, smooth ? normal.y : normals[c->z - 1]->y, smooth ? normal.z : normals[c->z - 1]->z, c->y ? uvCoords[c->y - 1]->x : 0.002f, c->y ? uvCoords[c->y - 1]->y : 0.002f };
			vertexData.insert(vertexData.end(), v1, v1 + 8);
			vertexData.insert(vertexData.end(), v2, v2 + 8);
			vertexData.insert(vertexData.end(), v3, v3 + 8);
			triangles++;
		}
	}

	faces.clear();
	vertices.clear();
	normals.clear();
	uvCoords.clear();

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

const Coord& OBJ::normalize(const shared_ptr<Coord>& a, const shared_ptr<Coord>& b, const shared_ptr<Coord>& c) {
	double u1, u2, u3, v1, v2, v3;

	u1 = b->x - a->x;
	u2 = b->y - a->y;
	u3 = b->z - a->z;
	v1 = c->x - a->x;
	v2 = c->y - a->y;
	v3 = c->z - a->z;

	return Coord(u2 * v3 - u3 * v2, u3 * v1 - u1 * v3, u1 * v2 - u2 * v1, 0, 0);
}