#pragma once

class Coord {
public:
	double x, y, z;		// model coords
	double u, v;			// texture coords

	Coord();
	Coord(double x, double y, double z, double u, double v);
	Coord(Coord &coord);

	void makeOBJTex();
	void makeTexCoord();
	void makeVertex();
	void makeNormal();
	double xzDistance(Coord* c);
	double xyDistance(Coord* c);
	double yzDistance(Coord* c);
	double xyzDistance(Coord* c);
	double uvDistance(Coord* c);
	bool operator==(Coord* c);
	void describe();
};