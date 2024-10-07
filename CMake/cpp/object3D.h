#pragma once
#include "SpaceObject.h"
#include "OBJ.h"
#include "coord.h"

class Object3D : public SpaceObject {
public:
	vector<shared_ptr<OBJ>> objects;
	int price;				// how much it costs to buy

	Object3D();
	void loadOBJ(string file);	// load OBJ file
	void draw();	// draw the object

	static vector<string> split(string str, char delimiter);
	static vector<string> split(string str, string delimiter);
};