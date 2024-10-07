#include <math.h>

#include "spaceobject.h"
#include "planet.h"
#include "controller.h"
#include "texture.h"
#include <glm/ext/matrix_transform.hpp>

// static variables
shared_ptr<SpaceObject> SpaceObject::SPACEOBJECT_ORIGIN;
Sphere* SpaceObject::sphereObj;
Controller* sys;

constexpr double HOLE_FACTOR = 1.67;
constexpr double SCALE_FACTOR = 1.1;

void SpaceObject::initialize() {
	SPACEOBJECT_ORIGIN = make_shared<SpaceObject>();
	sphereObj = new Sphere(20, 20);
}

SpaceObject::SpaceObject() {
	texture.resize(1);
	pos = glm::vec3(0);
	vUp = glm::vec3(0, 1, 0);
	vForward = glm::vec3(1, 0, 0);
	vRight = glm::vec3(0, 0, 1);
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

void SpaceObject::setOrbit(shared_ptr<SpaceObject> center) {
	centerOfRotation = center;
	if (center != NULL) {
		double m = centerOfRotation->mass;
		theta = atan2(pos.z - centerOfRotation->pos.z, pos.x - centerOfRotation->pos.x) * 180 / pi;
		distance = sqrt(pow(pos.x - centerOfRotation->pos.x, 2) + pow(pos.z - centerOfRotation->pos.z, 2));
		w = sqrt(sys->GRAVITY * m / (distance * distance));
	}
}

void SpaceObject::bounce(shared_ptr<SpaceObject> sphere) {
	double distAdd = 0;
	bool isPlanet = dynamic_pointer_cast<Planet>(sphere).get() != NULL;

	if (this == sys)
		distAdd = isPlanet ? .1 : .05;
	if (!(sqr(sphere->pos.x - pos.x) + sqr(sphere->pos.z - pos.z) + sqr(sphere->pos.y - pos.y) > sqr(size + sphere->size + distAdd))) {	// they touch
		double m1 = mass;
		double m2 = isPlanet ? 1E6 : sphere->mass;
		double dvx2, dvy2, dvz2;
		double A, B, dist;

		A = (sqr(sphere->pos.x - pos.x) + sqr(sphere->pos.z - pos.z) + sqr(sphere->pos.y - pos.y)) * m2 * (1.0f + m2 / m1);
		if (!isPlanet)
			B = 2 * m2 * ((sphere->pos.x - pos.x) * (sphere->velocity.x - velocity.x) + (sphere->pos.y - pos.y) * (sphere->velocity.y - velocity.y) + (sphere->pos.z - pos.z) * (sphere->velocity.z - velocity.z));
		else
			B = 2 * m2 * ((pos.x - sphere->pos.x) * velocity.x + (pos.y - sphere->pos.y) * velocity.y + (pos.z - sphere->pos.z) * velocity.z);

		dvx2 = -B / A * (sphere->pos.x - pos.x);
		dvy2 = -B / A * (sphere->pos.y - pos.y);
		dvz2 = -B / A * (sphere->pos.z - pos.z);

		velocity.x -= m2 / m1 * dvx2;
		velocity.y -= m2 / m1 * dvy2;
		velocity.z -= m2 / m1 * dvz2;

		dist = sqrt(sqr(sphere->pos.x - pos.x) + sqr(sphere->pos.z - pos.z) + sqr(sphere->pos.y - pos.y));
		dist = (size + sphere->size + distAdd - dist) / dist;

		if (!isPlanet) {
			sphere->velocity.x += dvx2;
			sphere->velocity.y += dvy2;
			sphere->velocity.z += dvz2;

			dist *= m1 / (m2 + m1);
			sphere->pos.x = sphere->pos.x + (sphere->pos.x - pos.x) * dist;
			sphere->pos.y = sphere->pos.y + (sphere->pos.y - pos.y) * dist;
			sphere->pos.z = sphere->pos.z + (sphere->pos.z - pos.z) * dist;
			dist *= m2 / m1;
		}

		pos.x += (pos.x - sphere->pos.x) * dist;
		pos.y += (pos.y - sphere->pos.y) * dist;
		pos.z += (pos.z - sphere->pos.z) * dist;
	}
}

void SpaceObject::position() {
	//if (sys->viewStyle == 0) {
	//	glLoadIdentity();
	//	glRotated(-sys->pitch, 1, 0, 0);
	//	glRotated(-sys->roll, 0, 0, 1);
	//	glRotated(-sys->angle, 0, 1, 0);			// sys angle
	//	glTranslated(x - sys->x, y - sys->y, sys->z - z);	// position self
	//	glRotated(angle, 0, 1, 0);				// rotate to own angle
	//	glRotated(roll, 0, 0, 1);
	//	glRotated(pitch, 1, 0, 0);
	//}
	//else if (sys->viewStyle == 1) {
	//	glLoadIdentity();
	//	glRotated(-sys->angle, 0, 0, 1);
	//	glTranslated((x - sys->x), (z - sys->z), -15 - sys->y + y);
	//	glRotated(angle, 0, 0, 1);
	//	glRotated(90, 1, 0, 0);
	//}
	//else if (sys->viewStyle == 2) {
	//	glLoadIdentity();
	//	glRotated(sys->pitch, 1, 0, 0);
	//	glRotated(sys->roll, 0, 0, 1);
	//	glRotated(180 - sys->angle, 0, 1, 0);
	//	glTranslated(x - sys->x, y - sys->y, sys->z - z);
	//	glRotated(angle, 0, 1, 0);
	//	glRotated(roll, 0, 0, 1);
	//	glRotated(pitch, 1, 0, 0);
	//}
	//else if (sys->viewStyle == 3) {
	//	glLoadIdentity();
	//	glTranslated(0, -sys->size, -5 * sys->size);
	//	glRotated(sys->deltaRot / 10, 0, 1, 0);
	//	glRotated(sys->deltaPitch / 10, 1, 0, 0);
	//	glRotated(-sys->pitch, 1, 0, 0);
	//	glRotated(-sys->roll, 0, 0, 1);
	//	glRotated(-sys->angle, 0, 1, 0);
	//	glTranslated(x - sys->x, y - sys->y, sys->z - z);
	//	glRotated(angle, 0, 1, 0);
	//	glRotated(roll, 0, 0, 1);
	//	glRotated(pitch, 1, 0, 0);
	//}
}

double SpaceObject::getDistance(shared_ptr<SpaceObject> other) {
	return sqrt(pow(pos.x - other->pos.x, 2) + pow(pos.z - other->pos.z, 2) + pow(pos.y - other->pos.y, 2));
}

glm::vec3 SpaceObject::globalToLocal(const glm::vec3& v) {
	glm::mat4 transform = glm::lookAt(pos, pos + vForward, vUp);
	return glm::vec3(transform * glm::vec4(v, 1));
}

bool SpaceObject::visible() {
	glm::vec3 local = sys->globalToLocal(pos);
	if (sys->viewStyle == 0) {
		if (local.x < -size * 1.9)
			return false;
	}
	else if (sys->viewStyle == 2) {
		if (local.x > size * 1.9)
			return false;
	}
	return true;
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