#include "asteroid.h"
#include "controller.h"
#include <glm/ext/matrix_transform.hpp>

#define sqr(x) ((x)*(x))

GLuint asteroidList;

void Asteroid::initialize() {
	asteroidList = glGenLists(1);
	/*glNewList(asteroidList, GL_COMPILE);
	gluSphere(SpaceObject::sphereObj, 1.0, 10, 10);
	glEndList();*/
}

Asteroid::Asteroid() {
	reset();
}

void Asteroid::reset() {
	size = .1-.1*sin((pi*rand())/RAND_MAX);
	pos = glm::vec3(0);
	pos.x = (100.0f * rand()) / RAND_MAX - 50;
	pos.z = (100.0f * rand()) / RAND_MAX - 50;
	pos.y = (30.0f * rand()) / RAND_MAX - 15;
	mass = size * 5000;
	velocity.x = ((float)rand()) / RAND_MAX - .5;
	velocity.z = ((float)rand()) / RAND_MAX - .5;
	velocity.y = ((float)rand()) / RAND_MAX - .5;
	deltaRot = -400 * size + 10;
}

void Asteroid::update() {
	pos.x += velocity.x * sys->FACTOR;
	pos.z += velocity.z * sys->FACTOR;
	pos.y += velocity.y * sys->FACTOR;
	MAX_VELOCITY = sqrt(velocity.x * velocity.x + velocity.z * velocity.z + velocity.y * velocity.y);
	if (MAX_VELOCITY > 4) {
		velocity.x *= 4 / MAX_VELOCITY;
		velocity.z *= 4 / MAX_VELOCITY;
		velocity.y *= 4 / MAX_VELOCITY;
	}
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), (float)deltaRot * sys->FACTOR, vUp);
	vRight = rot * glm::vec4(vRight, 1);
	vForward = rot * glm::vec4(vForward, 1);
	collide(sys->planets);
}

void Asteroid::draw() {
	if( !visible() )
		return;

	position();
	glBindTexture(GL_TEXTURE_2D, sys->texture[ASTEROID_TEXTURE]);
	glRotatef(-90, 1, 0, 0);
	glScalef(size, size, size);
	glCallList(asteroidList);
	if (sqr(pos.x - sys->pos.x) + sqr(pos.z - sys->pos.z) + sqr(pos.y - sys->pos.y) > 2500) {	// out of range
		reset();
	}
}