#pragma once

class Coord {
public:
	double x, y, z;		// model coords
	double u, v;			// texture coords

	Coord();
	Coord(double x, double y, double z, double u, double v);
	Coord(const Coord &coord);

	double xzDistance(Coord* c);
	double xyDistance(Coord* c);
	double yzDistance(Coord* c);
	double xyzDistance(Coord* c);
	double uvDistance(Coord* c);
	bool operator==(Coord* c);
	void describe();
};