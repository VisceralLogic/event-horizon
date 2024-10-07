#pragma once
#include "coord.h"
#include <vector>
#include <string>
#include <glad/glad.h>
#include <memory>

using namespace std;

class OBJ {
public:
	vector<shared_ptr<Coord>> vertices;
	vector<shared_ptr<Coord>> normals;
	vector<shared_ptr<Coord>> uvCoords;
	vector<vector<shared_ptr<Coord>>> faces;
	string name;
	string mtllib;
	string mtl;
	unsigned long texture;
	bool smooth = false;
	GLuint VBO, VAO;
	GLuint material;
	int triangles;

	~OBJ();
	void generateBuffers();
	static const Coord& normalize(const shared_ptr<Coord>& a, const shared_ptr<Coord>& b, const shared_ptr<Coord>& c);
};