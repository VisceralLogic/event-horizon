#include <cstdlib>
#include <ctime>

#include "ai.h"
#include "controller.h"
#include "cargo.h"
#include "mod.h"

void AI::initWithShip(shared_ptr<Spaceship> ship) {
	name = ship->name;
	MAX_VELOCITY = ship->MAX_VELOCITY;
	ANGULAR_ACCELERATION = ship->ANGULAR_ACCELERATION;
	MAX_ANGULAR_VELOCITY = ship->MAX_ANGULAR_VELOCITY;
	texture = ship->texture;
	mass = ship->mass;
	size = ship->size;
	thrust = ship->thrust;
	cargoSpace = ship->cargoSpace;
	modSpace = ship->modSpace;
	money = rand() % (ship->price/4);
	for (auto& el : ship->initData.items() ) {
		int num = el.value();
		for (int i = 0; i < num; i++) {
			addMod(static_pointer_cast<Mod>(Controller::componentNamed(el.key())));
		}
	}
	shields = ship->shields;
	maxShield = ship->shields;
	armor = ship->armor;
	maxArmor = ship->armor;
	fuel = ship->fuel;
	maxFuel = ship->fuel;
	primaryStraight = ship->primaryStraight;
	primaryTurret = ship->primaryTurret;
	secondaryStraight = ship->secondaryStraight;
	secondaryTurret = ship->secondaryTurret;
	if ( ship->cargo.size() ) {
		cargo = ship->cargo;
		for(auto& c : cargo) {
			cargoSpace -= c.size;
			mass += c.size;
		}
	}
	ACCELERATION = thrust / (mass * compensation);
	texNum = ship->texNum;
	rechargeRate = ship->rechargeRate;
	objects = ship->objects;
}

void AI::update() {
	if (shields <= 0 && armor <= 0) {
		if (startExplode == 0) {
			startExplode = sys->t;
		}
		else if (sys->t - startExplode >= 5) {
			state = DEAD;
			if (sys->selection == shared_from_this()) {
				sys->selection.reset();
				sys->shipIndex = -1;
			}
		}
		Spaceship::update();
		return;
	}
	else if (armor <= maxArmor / 4) {	// DISABLED
		state = DISABLED;
		slow = true;
		autopilot = false;
		orbit = false;
		centerOfRotation.reset();
		Spaceship::update();
		return;
	}

	switch (goal) {
	case DO_LAND:
		actionLand();
		break;
	case DO_LEAVE:
		actionLeave();
		break;
	case DO_ATTACK:
		actionAttack();
		break;
	case DO_LOITER:
		actionLoiter();
		break;
	case DO_ESCORT:
		actionEscort();
		break;
	default:
		goal = DO_LEAVE;
	}

	navigate(sys->planets);

	Spaceship::update();
}

void AI::doLand() {
	if (curPlanet) {
		float d = sqrt(sqr(pos.x - curPlanet->pos.x) + sqr(pos.z - curPlanet->pos.z) + sqr(pos.y - curPlanet->pos.y));
		if (d <= curPlanet->size + size + 0.15) {
			state = DEAD;
			if (sys->selection.get() == this) {
				sys->selection = nullptr;
				sys->shipIndex = -1;
			}
		}
	}
}

void AI::draw() {
	if( !visible() )
		return;
	if (shields <= 0 && armor <= 0) {
		if (sys->t - startExplode < 2.5)
			drawExplosionSize(sin((sys->t - startExplode) * pi / 2.5), 0.5, 0);
		if (sys->t - startExplode < 4)
			drawExplosionSize(sin((sys->t - startExplode - 1.5) * pi / 2.5), 0, 0.5);
		drawExplosionSize(sin((sys->t - startExplode - 2) * pi / 3), 0, 0);
		if (sys->t - startExplode > 2.5)
			return;
	}
	position();
	Spaceship::draw();
}

void AI::drawExplosionSize(float size, float x, float z) {
	//glDisable(GL_LIGHTING);
	//glBindTexture(GL_TEXTURE_2D, sys->texture[FIRE_TEXTURE]);
	//glTranslatef(x, 0, z);
	//glRotatef(90, 1, 0, 0);
	//glRotatef(50 * sys->t, 0, 0, 1);
	//gluSphere(sphereObj, _size, 20, 20);
	//glRotatef(-50 * sys->t, 0, 0, 1);
	//glRotatef(-90, 1, 0, 0);
	//glTranslatef(-x, 0, -z);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glEnable(GL_LIGHTING);
}

void AI::navigate(vector<shared_ptr<Planet>>& objects) {
	for (shared_ptr<Planet>& planet : objects) {
		float _distance, dotProduct;
		
		if (planet.get() == target.get())
			return;

		_distance = sqrt(sqr(planet->pos.x - pos.x) + sqr(planet->pos.z - pos.z) + sqr(planet->pos.y - pos.y));
		dotProduct = velocity.x*(planet->pos.x - pos.x) + velocity.z * (planet->pos.z - pos.z) + velocity.y * (planet->pos.y - pos.y);
		if ((dotProduct > 0) && (_distance < 5 + size + planet->size)) {
			// we're approaching it and it is less than 5 units distance
			float approach = dotProduct / _distance;	// length of velocity vector point at ob
			float theta = asin(approach / sqrt(sqr(velocity.x) + sqr(velocity.z))) + pi * angle / 180;

			_distance -= size + planet->size;
			_distance = 1. - 1. / _distance / _distance;
			if (_distance < 0.1)
				_distance = 0.1;
			_distance *= MAX_VELOCITY;
			if (approach > _distance) {
				slow = true;
				if (pos.y >= planet->pos.y)
					up = true;
				else
					down = true;
			}
		}
	}
}

void AI::actionLand() {
	if (!target) {
		float num = rand() % sys->planets.size();
		target = sys->planets[num];
		curPlanet = static_pointer_cast<Planet>(target);
	}

	autopilot = true;
	distance = sqrt(sqr(pos.x - curPlanet->pos.x) + sqr(pos.z - curPlanet->pos.z) + sqr(pos.y - curPlanet->pos.y)) - curPlanet->size - size;
	if (!centerOfRotation)
		orbit = true;
	forward = true;
	if (distance < 1.5) {
		land = true;
	}
	else {
		if (sqr(deltaRot) > sqr(MAX_ANGULAR_VELOCITY / 2))
			slow = true;
		else
			forward = true;
	}
}

void AI::actionLeave() {
	if (!target) {
		target = sys->system->links[rand() % sys->system->links.size()];
		angle = 180. / pi * atan2(static_pointer_cast<Solarsystem>(target)->z - sys->system->z, static_pointer_cast<Solarsystem>(target)->x - sys->system->x);
	}

	forward = true;
	if (sqr(pos.x) + sqr(pos.z) + sqr(pos.y) > jumpDistance || sys->system->planets.size() == 0) {
		hyperspace = true;
	}
}

void AI::actionAttack() {
	float dist;

	autopilot = true;
	if (sqr(deltaRot) > sqr(MAX_ANGULAR_VELOCITY / 2))
		slow = true;
	else
		forward = true;

	dist = sqrt(sqr(pos.x - static_pointer_cast<Spaceship>(target)->pos.x) + sqr(pos.z - static_pointer_cast<Spaceship>(target)->pos.z) + sqr(pos.y - static_pointer_cast<Spaceship>(target)->pos.y));
	if (dist < 5) {
		fire = true;
		if (dist < 2)
			slow = true;
	}
	if( ++secondaryIndex >= secondary.size() )
		secondaryIndex = -1;
	fireSecondary = true;
}

void AI::actionLoiter() {
	slow = true;
}

void AI::actionEscort() {
	selection = static_pointer_cast<AI>(escortee);
	autopilot = true;
	if( sqr(pos.x - escortee->pos.x) + sqr(pos.z - escortee->pos.z) + sqr(pos.y - escortee->pos.y) > sqr(5 * size + escortee->size)) {
		forward = true;
	}
}