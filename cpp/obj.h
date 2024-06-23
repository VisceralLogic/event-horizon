#pragma once
#include "coord.h"
#include <vector>
#include <string>

using namespace std;

class OBJ {
public:
	vector<Coord*> vertices;
	vector<Coord*> normals;
	vector<Coord*> uvCoords;
	vector<vector<Coord*>> faces;
	string name;
	string mtllib;
	string mtl;
	unsigned long texture;
	bool smooth = false;

	~OBJ() {
		for( int i = 0; i < vertices.size(); i++ )
			delete vertices[i];
		for( int i = 0; i < normals.size(); i++ )
			delete normals[i];
		for( int i = 0; i < uvCoords.size(); i++ )
			delete uvCoords[i];
		for( int i = 0; i < faces.size(); i++ ) {
			for( int j = 0; j < faces[i].size(); j++ )
				delete faces[i][j];
		}
	}
};