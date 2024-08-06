#include "asteroid.h"
#include "controller.h"

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
	x = (100.0f * rand()) / RAND_MAX - 50;
	z = (100.0f * rand()) / RAND_MAX - 50;
	y = (30.0f * rand()) / RAND_MAX - 15;
	mass = size * 5000;
	speedx = ((float)rand()) / RAND_MAX - .5;
	speedz = ((float)rand()) / RAND_MAX - .5;
	speedy = ((float)rand()) / RAND_MAX - .5;
	deltaRot = -400 * size + 10;
}

void Asteroid::update() {
	x += speedx * sys->FACTOR;
	z += speedz * sys->FACTOR;
	y += speedy * sys->FACTOR;
	MAX_VELOCITY = sqrt(speedx * speedx + speedz * speedz + speedy * speedy);
	if (MAX_VELOCITY > 4) {
		speedx *= 4 / MAX_VELOCITY;
		speedz *= 4 / MAX_VELOCITY;
		speedy *= 4 / MAX_VELOCITY;
	}
	angle += deltaRot * sys->FACTOR;
	collide((SpaceObject**)sys->planets.data(), sys->planets.size());
}

void Asteroid::draw() {
	if( !visible() )
		return;

	position();
	glBindTexture(GL_TEXTURE_2D, sys->texture[ASTEROID_TEXTURE]);
	glRotatef(-90, 1, 0, 0);
	glScalef(size, size, size);
	glCallList(asteroidList);
	if (sqr(x - sys->x) + sqr(z - sys->z) + sqr(y - sys->y) > 2500) {	// out of range
		reset();
	}
}