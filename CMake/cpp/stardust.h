#pragma once

#include "spaceobject.h"

class StarDust : public SpaceObject {
	static GLuint vao, vbo;

public:

	static void initialize();
	static void update();

	static vector<shared_ptr<StarDust>> dust;
	void init();
	void draw();
};