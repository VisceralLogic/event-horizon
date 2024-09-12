#pragma once
#include "SpaceObject.h"
#include "OBJ.h"
#include "coord.h"

class Object3D : public SpaceObject {
public:
	vector<Coord*> tri;	// arrays of coords defining tri, quad, poly
	vector<Coord*> quad;
	vector<vector<Coord*>> poly;	// array of array of coord
	GLuint listNum;			// number of lists
	vector<GLuint> list;			// gl drawing lists
	bool obj;				// is this a WaveFront OBJ?
	vector<OBJ*> objects;
	int price;				// how much it costs to buy

	Object3D();
	~Object3D();
	void load(const char *data, string tex);	// load 3D data from file
	void drawObject();	// draw self
	void listDraw();		// make gl calls for drawing list
	void objListDraw(OBJ* o);	// gl calls for OBJ file
	void loadOBJ(string file);	// load OBJ file

	static void makeNormal(vector<Coord*>& array, int index);
	static void normalize(Coord* a, Coord* b, Coord* c);
	static vector<string> split(string str, char delimiter);
	static vector<string> split(string str, string delimiter);
};