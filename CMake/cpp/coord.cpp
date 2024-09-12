#include "coord.h"
#include <math.h>
#include <glad/glad.h>
#include <iostream>

Coord::Coord() {
	x = 0;
	y = 0;
	z = 0;
	u = 0;
	v = 0;
}

Coord::Coord(double x, double y, double z, double u, double v) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->u = u;
	this->v = v;
}

Coord::Coord(const Coord &coord) {
	x = coord.x;
	y = coord.y;
	z = coord.z;
	u = coord.u;
	v = coord.v;
}

void Coord::makeOBJTex() {
	glTexCoord2d(x, y);
}

void Coord::makeTexCoord() {
	glTexCoord2d(u, 1 - v);
}

void Coord::makeVertex() {
	glVertex3d(x, y, z);
}

void Coord::makeNormal() {
	glNormal3d(x, y, z);
}

double Coord::xzDistance(Coord* c) {
	return sqrt(pow(x - c->x, 2) + pow(z - c->z, 2));
}

double Coord::xyDistance(Coord* c) {
	return sqrt(pow(x - c->x, 2) + pow(y - c->y, 2));
}

double Coord::yzDistance(Coord* c) {
	return sqrt(pow(y - c->y, 2) + pow(z - c->z, 2));
}

double Coord::xyzDistance(Coord* c) {
	return sqrt(pow(x - c->x, 2) + pow(y - c->y, 2) + pow(z - c->z, 2));
}

double Coord::uvDistance(Coord* c) {
	return sqrt(pow(u - c->u, 2) + pow(v - c->v, 2));
}

bool Coord::operator==(Coord* c) {
	return (x == c->x && y == c->y && z == c->z);
}

void Coord::describe() {
	std::cout << "<" << x << ", " << y << ", " << z << ", " << u << ", " << v << ">\n";
}
