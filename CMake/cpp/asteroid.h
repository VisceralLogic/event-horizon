#pragma once
#include "object3d.h"

class Asteroid : public Object3D {
public:
	static void initialize();
	Asteroid();
	void draw();
	void update();
	void reset();
};