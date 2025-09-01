#include <math.h>

#include "spaceobject.h"
#include "planet.h"
#include "controller.h"
#include "texture.h"
#include <glm/ext/matrix_transform.hpp>

// static variables
shared_ptr<SpaceObject> SpaceObject::SPACEOBJECT_ORIGIN;
Sphere* SpaceObject::sphereObj;
GLuint SpaceObject::bracketVAO, SpaceObject::bracketVBO;
Controller* sys;

constexpr double HOLE_FACTOR = 1.67;
constexpr double SCALE_FACTOR = 1.1;

void SpaceObject::initialize() {
	SPACEOBJECT_ORIGIN = make_shared<SpaceObject>();
	sphereObj = new Sphere(20, 20);

	float vertex[] = {
		1/HOLE_FACTOR, 1, 1,
		-1 / HOLE_FACTOR, 1, 1,
		1, 1 / HOLE_FACTOR, 1,
		1, -1 / HOLE_FACTOR, 1,
		1, 1, 1 / HOLE_FACTOR,
		1, 1, -1 / HOLE_FACTOR,

		-1 / HOLE_FACTOR, -1, -1,
		1 / HOLE_FACTOR, -1, -1,
		-1, -1 / HOLE_FACTOR, -1,
		-1, 1 / HOLE_FACTOR, -1,
		-1, -1, -1 / HOLE_FACTOR,
		-1, -1, 1 / HOLE_FACTOR,

		-1, 1 / HOLE_FACTOR, 1,
		-1, -1 / HOLE_FACTOR, 1,
		-1, 1, 1 / HOLE_FACTOR,
		-1, 1, -1 / HOLE_FACTOR,

		1 / HOLE_FACTOR, -1, 1,
		-1 / HOLE_FACTOR, -1, 1,
		1, -1, 1 / HOLE_FACTOR,
		1, -1, -1 / HOLE_FACTOR,

		1 / HOLE_FACTOR, 1, -1,
		-1 / HOLE_FACTOR, 1, -1,

		1, 1 / HOLE_FACTOR, -1,
		1, -1 / HOLE_FACTOR, -1
	};
	glGenBuffers(1, &bracketVBO);
	glGenVertexArrays(1, &bracketVAO);
	glBindVertexArray(bracketVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bracketVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
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
	position();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(bracketVAO);
	glDrawArrays(GL_LINES, 0, 42);
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
	//glm::mat4 model = glm::lookAt(pos, pos + vForward, vUp);
	/*glm::mat4 model = glm::mat4(1.0f);
	model[0][0] = vRight.x;
	model[1][0] = vRight.y;
	model[2][0] = vRight.z;

	model[0][1] = vUp.x;
	model[1][1] = vUp.y;
	model[2][1] = vUp.z;

	model[0][2] = -vForward.x;
	model[1][2] = -vForward.y;
	model[2][2] = -vForward.z;

	model[3][0] = pos.x;
	model[3][1] = pos.y;
	model[3][2] = pos.z;
	model = glm::scale(model, glm::vec3(size));
	Controller::shader->setUniformMat4("model", model);*/
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::rotate(model, float(pitch * pi / 180), glm::vec3(1, 0, 0));
	model = glm::rotate(model, float((angle-90) * pi / 180), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(size));
	Controller::shader->setUniformMat4("model", model);
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
		if (local.z > size)
			return false;
	}
	else if (sys->viewStyle == 2) {
		if (local.z < size)
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