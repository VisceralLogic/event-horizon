#include <math.h>

#include "spaceobject.h"
#include "planet.h"
#include "controller.h"
#include "texture.h"

// static variables
SpaceObject* SpaceObject::SPACEOBJECT_ORIGIN;
GLUquadricObj* SpaceObject::sphereObj;
Controller* SpaceObject::sys;

constexpr double HOLE_FACTOR = 1.67;
constexpr double SCALE_FACTOR = 1.1;

void SpaceObject::initialize() {
	SPACEOBJECT_ORIGIN = new SpaceObject();
	sphereObj = gluNewQuadric();
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricOrientation(sphereObj, GLU_OUTSIDE);
}

void SpaceObject::loadTextures(const vector<string>& files) {
	texture.resize(texNum);
	for (int i = 0; i < texNum; i++) {
		Texture::loadTexture((char*)files[i].c_str(), &texture[i], NULL, NULL);
	}
}

void SpaceObject::bracket() {
	this->position();
	glBindTexture(GL_TEXTURE_2D, 0);
	size *= SCALE_FACTOR;
	glBegin(GL_LINES);
	glVertex3f(size / HOLE_FACTOR, size, size);
	glVertex3f(-size / HOLE_FACTOR, size, size);
	glVertex3f(size, size / HOLE_FACTOR, size);
	glVertex3f(size, -size / HOLE_FACTOR, size);
	glVertex3f(size, size, size / HOLE_FACTOR);
	glVertex3f(size, size, -size / HOLE_FACTOR);

	glVertex3f(-size / HOLE_FACTOR, -size, -size);
	glVertex3f(size / HOLE_FACTOR, -size, -size);
	glVertex3f(-size, -size / HOLE_FACTOR, -size);
	glVertex3f(-size, size / HOLE_FACTOR, -size);
	glVertex3f(-size, -size, -size / HOLE_FACTOR);
	glVertex3f(-size, -size, size / HOLE_FACTOR);

	glVertex3f(-size, size / HOLE_FACTOR, size);
	glVertex3f(-size, -size / HOLE_FACTOR, size);
	glVertex3f(-size, size, size / HOLE_FACTOR);
	glVertex3f(-size, size, -size / HOLE_FACTOR);

	glVertex3f(size / HOLE_FACTOR, -size, size);
	glVertex3f(-size / HOLE_FACTOR, -size, size);
	glVertex3f(size, -size, size / HOLE_FACTOR);
	glVertex3f(size, -size, -size / HOLE_FACTOR);

	glVertex3f(size / HOLE_FACTOR, size, -size);
	glVertex3f(-size / HOLE_FACTOR, size, -size);

	glVertex3f(size, size / HOLE_FACTOR, -size);
	glVertex3f(size, -size / HOLE_FACTOR, -size);
	glEnd();
	size /= SCALE_FACTOR;
	glColor3f(1, 1, 1);
}

void SpaceObject::setOrbit(SpaceObject* center) {
	centerOfRotation = center;
	if (center != NULL) {
		double m = centerOfRotation->mass;
		theta = atan2(z - centerOfRotation->z, x - centerOfRotation->x) * 180 / pi;
		distance = sqrt(pow(x - centerOfRotation->x, 2) + pow(z - centerOfRotation->z, 2));
		w = sqrt(sys->GRAVITY * m / (distance * distance));
	}
}

// Perform collision function with each object in array
void SpaceObject::collide(SpaceObject **array, int count) {
	for (int i = 0; i < count; i++) {
		SpaceObject* a = array[i];
		if (a != this)
			bounce(a);
	}
}

void SpaceObject::bounce(SpaceObject* sphere) {
	double distAdd = 0;
	bool isPlanet = dynamic_cast<Planet*>(sphere) != NULL;

	if (this == sys)
		distAdd = isPlanet ? .1 : .05;
	if (!(pow(sphere->x - x, 2) + pow(sphere->z - z, 2) + pow(sphere->y - y, 2) > pow(size + sphere->size + distAdd, 2))) {	// they touch
		double m1 = mass;
		double m2 = isPlanet ? 1E6 : sphere->mass;
		double dvx2, dvy2, dvz2;
		double A, B, dist;

		A = (pow(sphere->x - x, 2) + pow(sphere->z - z, 2) + pow(sphere->y - y, 2)) * m2 * (1.0f + m2 / m1);
		if (!isPlanet)
			B = 2 * m2 * ((sphere->x - x) * (sphere->speedx - speedx) + (sphere->y - y) * (sphere->speedy - speedy) + (sphere->z - z) * (sphere->speedz - speedz));
		else
			B = 2 * m2 * ((x - sphere->x) * speedx + (y - sphere->y) * speedy + (z - sphere->z) * speedz);

		dvx2 = -B / A * (sphere->x - x);
		dvy2 = -B / A * (sphere->y - y);
		dvz2 = -B / A * (sphere->z - z);

		speedx -= m2 / m1 * dvx2;
		speedy -= m2 / m1 * dvy2;
		speedy -= m2 / m1 * dvz2;

		dist = sqrt(pow(sphere->x - x, 2) + pow(sphere->z - z, 2) + pow(sphere->y - y, 2));
		dist = (size + sphere->size + distAdd - dist) / dist;

		if (!isPlanet) {
			sphere->speedx += dvx2;
			sphere->speedy += dvy2;
			sphere->speedz += dvz2;

			dist *= m1 / (m2 + m1);
			sphere->x += (sphere->x - x) * dist;
			sphere->y += (sphere->y - y) * dist;
			sphere->z += (sphere->z - z) * dist;
			dist *= m2 / m1;
		}

		x += (x - sphere->x) * dist;
		y += (y - sphere->y) * dist;
		z += (z - sphere->z) * dist;
	}
}

void SpaceObject::position() {
	if (sys->viewStyle == 0) {
		glLoadIdentity();
		glRotated(-sys->pitch, 1, 0, 0);
		glRotated(-sys->roll, 0, 0, 1);
		glRotated(-sys->angle, 0, 1, 0);			// sys angle
		glTranslated(x - sys->x, y - sys->y, sys->z - z);	// position self
		glRotated(angle, 0, 1, 0);				// rotate to own angle
		glRotated(roll, 0, 0, 1);
		glRotated(pitch, 1, 0, 0);
	}
	else if (sys->viewStyle == 1) {
		glLoadIdentity();
		glRotated(-sys->angle, 0, 0, 1);
		glTranslated((x - sys->x), (z - sys->z), -15 - sys->y + y);
		glRotated(angle, 0, 0, 1);
		glRotated(90, 1, 0, 0);
	}
	else if (sys->viewStyle == 2) {
		glLoadIdentity();
		glRotated(sys->pitch, 1, 0, 0);
		glRotated(sys->roll, 0, 0, 1);
		glRotated(180 - sys->angle, 0, 1, 0);
		glTranslated(x - sys->x, y - sys->y, sys->z - z);
		glRotated(angle, 0, 1, 0);
		glRotated(roll, 0, 0, 1);
		glRotated(pitch, 1, 0, 0);
	}
	else if (sys->viewStyle == 3) {
		glLoadIdentity();
		glTranslated(0, -sys->size, -5 * sys->size);
		glRotated(sys->deltaRot / 10, 0, 1, 0);
		glRotated(sys->deltaPitch / 10, 1, 0, 0);
		glRotated(-sys->pitch, 1, 0, 0);
		glRotated(-sys->roll, 0, 0, 1);
		glRotated(-sys->angle, 0, 1, 0);
		glTranslated(x - sys->x, y - sys->y, sys->z - z);
		glRotated(angle, 0, 1, 0);
		glRotated(roll, 0, 0, 1);
		glRotated(pitch, 1, 0, 0);
	}
}

double SpaceObject::getDistance(SpaceObject* other) {
	return sqrt(pow(x - other->x, 2) + pow(z - other->z, 2) + pow(y - other->y, 2));
}

void SpaceObject::localToGlobal() {
	double _theta = pitch * pi / 180;
	double phi = roll * pi / 180;
	double psi = angle * pi / 180;
	double X, Y, Z;

	X = _x * cos(_theta) - _y * sin(_theta);		// rotate theta
	Y = _x * sin(_theta) + _y * cos(_theta);
	_x = X;
	_y = Y;
	Y = _y * cos(phi) + _z * sin(phi);			// rotate phi
	Z = -_y * sin(phi) + _z * cos(phi);
	_y = Y;
	_z = Z;
	X = -_x * sin(psi) + _z * cos(psi);			// rotate yaw
	Z = _x * cos(psi) + _z * sin(psi);
	_x = X;
	_z = Z;
	_x += x;								// offset to current position
	_y += y;
	_z += z;
}

void SpaceObject::globalToLocal() {
	double _theta = pitch * pi / 180;
	double phi = roll * pi / 180;
	double psi = angle * pi / 180;
	double X, Y, Z;

	_x -= x;
	_y -= y;
	_z -= z;
	X = -_x * sin(psi) + _z * cos(psi);
	Z = _x * cos(psi) + _z * sin(psi);
	_x = X;
	_z = Z;
	Y = _y * cos(phi) - _z * sin(phi);
	Z = _y * sin(phi) + _z * cos(phi);
	_y = Y;
	_z = Z;
	X = _x * cos(_theta) + _y * sin(_theta);
	Y = -_x * sin(_theta) + _y * cos(_theta);
	_x = X;
	_y = Y;
}

void SpaceObject::calcAngles() {		// convert global direction vectors to angles
	double tX, tZ, a;

	if (rY == 1.0f) {
		angle = atan2(-fX, fZ) * 180. / pi;
		roll = 90;
		pitch = 0;
	}
	else if (rY == -1.0f) {
		angle = atan2(-fX, fZ) * 180. / pi;
			roll = -90;
		pitch = 0;
	}
	else if (rX != 0) {
		a = sqrt(1. + rZ * rZ / rX / rX);
		tX = -rZ / rX / a;
		tZ = 1.0f / a;
		a = tX * fX + tZ * fZ;
		if (a > 0) {
			if (a > 1)
				a = 1;
			angle = 180. / pi * atan2(-tX, tZ);
			pitch = 180. / pi * acos(a);
		}
		else {
			if (a < -1)
				a = -1;
			angle = 180. / pi * atan2(tX, -tZ);
			pitch = 180.0f - 180. / pi * acos(a);
			tX *= -1.0f;
			tZ *= -1.0f;
		}
		if (uX * tX + uZ * tZ > 0)
			pitch *= -1.0f;
		// rotate right 90 to find roll start location
		a = tZ;
		tZ = -tX;
		tX = a;
		a = rX * tX + rZ * tZ;
		if (a > 1)
			a = 1;
		if (a < -1)
			a = -1;
		roll = 180. / pi * acos(a);
		if (rY < 0)
			roll *= -1.0f;
	}
	else {
		angle = 90;
		pitch = 0;
		roll = atan2(rY, rZ) * 180. / pi;
	}
}

bool SpaceObject::visible() {
	sys->_x = x;
	sys->_y = y;
	sys->_z = z;
	sys->globalToLocal();
	if (sys->viewStyle == 0) {
		if (sys->_x < -size * 1.9)
			return false;
	}
	else if (sys->viewStyle == 2) {
		if (sys->_x > size * 1.9)
			return false;
	}
	return true;
}

double SpaceObject::getX() {
	return x;
}

void SpaceObject::setX(double x) {
	this->x = x;
}

double SpaceObject::getY() {
	return y;
}

void SpaceObject::setY(double y) {
	this->y = y;
}

double SpaceObject::getZ() {
	return z;
}

void SpaceObject::setZ(double z) {
	this->z = z;
}

double SpaceObject::getAngularAcceleration() {
	return ANGULAR_ACCELERATION;
}

void SpaceObject::setAngularAcceleration(double a) {
	ANGULAR_ACCELERATION = a;
}

double SpaceObject::getMaxAngularVelocity() {
	return MAX_ANGULAR_VELOCITY;
}

void SpaceObject::setMaxAngularVelocity(double v) {
	MAX_ANGULAR_VELOCITY = v;
}

double SpaceObject::getMaxVelocity() {
	return MAX_VELOCITY;
}

void SpaceObject::setMaxVelocity(double v) {
	MAX_VELOCITY = v;
}

int SpaceObject::getMass() {
	return mass;
}

void SpaceObject::setMass(int m) {
	mass = m;
}

string SpaceObject::getName() {
	return name;
}

void SpaceObject::setName(string n) {
	name = n;
}

map<string, void*>* SpaceObject::getFlags() {
	return flags;
}