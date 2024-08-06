#pragma once

class ShieldSpot {
public:
	float x, y, z, a, b, c, u, v, w;
	float start;
	float size;
	bool done;

	ShieldSpot(float x, float y, float z, float time, float size);
	void update(float percent);
};