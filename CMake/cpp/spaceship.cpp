#include "spaceship.h"
#include "controller.h"

#define sqr(x) ((x)*(x))

Spaceship::Spaceship() {
	secondaryIndex = -1;
	compensation = 1;
	jitter = 0;
	inertial = false;
}

Spaceship::~Spaceship() {
	//free(texture);
	//[weapons release] ;
	//[mods release] ;
	//[enemies release] ;
	//[secondary release] ;
	//[escorts release] ;

	//[super dealloc] ;
}

void Spaceship::update() {
	float jitterPitch, jitterRot;

	abSpeed = 0;
	useThrottle = true;
	if (inertial)
		useThrottle = false;
	if (left) {
		goLeft();
		left = false;
	}
	if (right) {
		goRight();
		right = false;
	}
	if (forward) {
		goForward();
		forward = false;
		useThrottle = false;
	}
	else if (throttledSpeed > 0)
		goForward();
	if (slow) {
		goSlow();
		slow = false;
	}
	if (autopilot) {
		doAutopilot();
	}
	if (orbit) {
		toggleOrbit();
		orbit = false;
	}
	if (hyperspace) {
		doHyperspace();
	}
	if (land) {
		doLand();
		land = false;
	}
	if (fire) {
		doFire(true);
		fire = false;
	}
	if (fireSecondary) {
		doFire(false);
		fireSecondary = false;
	}
	if (up) {
		goUp();
		up = false;
	}
	if (down) {
		goDown();
		down = false;
	}
	if (throttleUp) {
		throttle(true);
		throttleUp = false;
	}
	if (throttleDown) {
		throttle(false);
		throttleDown = false;
	}
	if (afterburner) {
		doAfterburner();
		afterburner = false;
	}

	if (centerOfRotation)		// if we're orbiting...
		doOrbit();

	x += speedx * sys->FACTOR;
	z += speedz * sys->FACTOR;
	y += speedy * sys->FACTOR;

	collide((SpaceObject**)sys->planets.data(), sys->planets.size());
	collide((SpaceObject**)sys->ships.data(), sys->ships.size());
	collide((SpaceObject**)sys->asteroids.data(), sys->asteroids.size());

	if (deltaRot > MAX_ANGULAR_VELOCITY)
		deltaRot = MAX_ANGULAR_VELOCITY;
	else if (deltaRot < -MAX_ANGULAR_VELOCITY)
		deltaRot = -MAX_ANGULAR_VELOCITY;
	if (deltaPitch > MAX_ANGULAR_VELOCITY)					// handle pitching motion
		deltaPitch = MAX_ANGULAR_VELOCITY;
	else if (deltaPitch < -MAX_ANGULAR_VELOCITY)
		deltaPitch = -MAX_ANGULAR_VELOCITY;

	if (abSpeed != 0) {
		if (sqr(speedx) + sqr(speedz) + sqr(speedy) > sqr(MAX_VELOCITY + abSpeed)) {
			float vel = sqrt(sqr(speedx) + sqr(speedy) + sqr(speedz));
			speedx *= (MAX_VELOCITY + abSpeed) / vel;
			speedy *= (MAX_VELOCITY + abSpeed) / vel;
			speedz *= (MAX_VELOCITY + abSpeed) / vel;
		}
	}
	else if (sqr(speedx) + sqr(speedz) + sqr(speedy) > (useThrottle ? sqr(throttledSpeed) : sqr(MAX_VELOCITY))) {
		float vel = sqrt(sqr(speedx) + sqr(speedz) + sqr(speedy));
		float speed;
		if (vel > (useThrottle ? throttledSpeed : MAX_VELOCITY) + ACCELERATION * sys->FACTOR)
			speed = vel - 1.1 * ACCELERATION * sys->FACTOR;
		else
			speed = useThrottle ? throttledSpeed : MAX_VELOCITY;
		speedx = speedx * speed / vel;
		speedz = speedz * speed / vel;
		speedy = speedy * speed / vel;
	}
	else if (useThrottle && sqr(speedx) + sqr(speedz) + sqr(speedy) < sqr(throttledSpeed)) {
		goForward();
	}

	// handle jitter
	jitterPitch = jitter * (0.5 - rand() * 1.0 / RAND_MAX);
	jitterRot = jitter * (0.5 - rand() * 1.0 / RAND_MAX);
	jitter = jitter * exp(-4.6 * sys->FACTOR);

	fX = cos(deltaRot * pi / 180. * sys->FACTOR + jitterRot);		// handle angle
	fY = 0;
	fZ = -sin(deltaRot * pi / 180. * sys->FACTOR + jitterRot);
	rX = sin(deltaRot * pi / 180. * sys->FACTOR + jitterRot);
	rY = 0;
	rZ = cos(deltaRot * pi / 180. * sys->FACTOR + jitterRot);		// end angle
	fY = sin(deltaPitch * pi / 180. * sys->FACTOR + jitterPitch);	// handle pitch
	_x = cos(deltaPitch * pi / 180. * sys->FACTOR + jitterPitch);		// normalize
	fX *= _x;
	fZ *= _x;						// end pitch
	uX = -fY * rZ;					// cross product
	uY = rZ * fX - fZ * rX;
	uZ = rX * fY - fX * rY;

	_x = rX;
	_y = rY;
	_z = rZ;
	localToGlobal();
	rX = _x - x;
	rY = _y - y;
	rZ = _z - z;
	_x = fX;
	_y = fY;
	_z = fZ;
	localToGlobal();
	fX = _x - x;
	fY = _y - y;
	fZ = _z - z;
	_x = uX;
	_y = uY;
	_z = uZ;
	localToGlobal();
	uX = _x - x;
	uY = _y - y;
	uZ = _z - z;
	calcAngles();

	if (state == ALIVE) {
		shields += rechargeRate * sys->FACTOR;
		if (shields > maxShield)
			shields = maxShield;
	}
}

void Spaceship::bracket() {
	glColor3f(0, 1, 0);
	if( sys->enemies.count(this) )
		glColor3f(1, 0, 0);
	else if (escortee == sys)
		glColor3f(1, 1, 0);
	if (state == DISABLED)
		glColor3f(0.5, 0.5, 0.5);
	SpaceObject::bracket();
}

void Spaceship::goLeft() {

}

void Spaceship::goRight() {

}

void Spaceship::goForward() {

}

void Spaceship::goSlow() {

}

void Spaceship::doAutopilot() {

}

void Spaceship::doOrbit() {

}

void Spaceship::toggleOrbit() {

}

void Spaceship::doHyperspace() {

}

void Spaceship::doLand() {

}

void Spaceship::doFire(bool primary) {

}

void Spaceship::throttle(bool forward) {

}

void Spaceship::doAfterburner() {

}

void Spaceship::goUp() {

}

void Spaceship::goDown() {

}