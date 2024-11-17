#include <iostream>
#include <fstream>
#include <sstream>

#include "spaceship.h"
#include "controller.h"
#include "plugin.h"
#include "cargo.h"
#include "aimerchant.h"
#include "aiagressor.h"
#include "aiwimp.h"
#include <glm/ext/matrix_transform.hpp>

#define sqr(x) ((x)*(x))

const float Spaceship::jumpDistance = 10000;

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

void Spaceship::registerFromDictionary(const json& dictionary) {
	shared_ptr<Spaceship> ship = make_shared<Spaceship>();
	json empty;
	if (!dictionary.contains("Data"))
		return;
	string dataFile = dictionary["Data"];
	if (dataFile.ends_with("obj") || dataFile.ends_with("OBJ")) {
		ship->loadOBJ((string)dictionary["Directory"] + dataFile);
	}
	ship->name = dictionary.contains("Name") ? dictionary["Name"] : dictionary["ID"];
	ship->ID = (string)dictionary["PluginName"] + ".ships." + (string)dictionary["ID"];
	ship->ANGULAR_ACCELERATION = dictionary.contains("AngularAcceleration") ? (double)dictionary["AngularAcceleration"] : 25;
	ship->MAX_ANGULAR_VELOCITY = dictionary.contains("MaxAngularVelocity") ? (double)dictionary["MaxAngularVelocity"] : 60;
	ship->thrust = dictionary.contains("Thrust") ? (float)dictionary["Thrust"] : 250;
	ship->MAX_VELOCITY = dictionary.contains("MaxSpeed") ? (double)dictionary["MaxSpeed"] : 2;
	ship->mass = dictionary.contains("Mass") ? (int)dictionary["Mass"] : 100;
	ship->size = dictionary.contains("Size") ? (float)dictionary["Size"] : 0.15f;
	ship->cargoSpace = dictionary.contains("CargoSpace") ? (int)dictionary["CargoSpace"] : 15;
	ship->modSpace = dictionary.contains("ModSpace") ? (int)dictionary["ModSpace"] : 20;
	if (dictionary.contains("Start")) {
		Plugin::plugins["default"]->spaceships["start"] = ship;
	}
	ship->flags = dictionary.contains("Flags") ? dictionary["Flags"] : empty;
	ship->price = dictionary.contains("Price") ? (int)dictionary["Price"] : 10000;
	ship->description = dictionary.contains("Description") ? dictionary["Description"] : "";
	ship->initData["Mods"] = dictionary.contains("Mods") ? dictionary["Mods"] : empty;
	ship->initData["Hardpoints"] = dictionary.contains("Hardpoints") ? dictionary["Hardpoints"] : empty;
	ship->shields = dictionary.contains("Shields") ? (int)dictionary["Shields"] : 30;
	ship->maxShield = ship->shields;
	ship->armor = dictionary.contains("Armor") ? (int)dictionary["Armor"] : 20;
	ship->maxArmor = ship->armor;
	ship->fuel = dictionary.contains("Fuel") ? (int)dictionary["Fuel"] : 5;
	ship->maxFuel = ship->fuel;
	ship->flagRequirements = dictionary.contains("FlagRequirements") ? dictionary["FlagRequirements"] : "";
	if( dictionary.contains("RechargeTime") )
		ship->rechargeRate = ship->maxShield / dictionary["RechargeTime"];
	/*
	ship->defaultAI = [[dictionary objectForKey:@"DefaultAI"] copy];
	ship->primaryStraight = [dictionary objectForKey:@"Hardpoints"];
	ship->cargo = [dictionary objectForKey:@"Cargo"];
	if( [dictionary objectForKey:@"FramePath"] )
		ship->frameFile = [[NSString alloc] initWithFormat:@"%@/%@", [[NSFileManager defaultManager] currentDirectoryPath], [dictionary objectForKey:@"FramePath"]];

		*/
	if (dictionary.contains("Replace"))
		ship->ID = dictionary["Replace"];
	vector<string> temp = split(ship->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->spaceships[temp[2]] = ship;
}

void Spaceship::finalize() {
	if (!initData.is_null()) {
		for (auto it = initData["Hardpoints"].begin(); it != initData["Hardpoints"].end(); it++) {
			vector<Coord>* array;
			if (it.key() == "PrimaryStraight") {
				array = &primaryStraight;
			} else if (it.key() == "PrimaryTurret") {
				array = &primaryTurret;
			} else if (it.key() == "SecondaryStraight") {
				array = &secondaryStraight;
			} else if (it.key() == "SecondaryTurret") {
				array = &secondaryTurret;
			}
			for( auto cit = it.value().begin(); cit != it.value().end(); cit++ ) {
				Coord coord;
				coord.x = cit.value()[0];
				coord.y = cit.value()[1];
				coord.z = cit.value()[2];
				array->push_back(coord);
			}
		}
	}
	initData.clear();
}

shared_ptr<Spaceship> Spaceship::newInstance() {
	shared_ptr<AI> ship;
	if( defaultAI == "MerchantAI" )
		ship = make_shared<AIMerchant>();
	else if (defaultAI == "AgressorAI")
		ship = make_shared<AIAgressor>();
	else /*if (defaultAI == "WimpAI")*/
		ship = make_shared<AIWimp>();
	ship->initWithShip(shared_from_this());
	ship->state = ALIVE;

	return ship;
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

	pos += velocity * sys->FACTOR;

	// handle jitter
	jitterPitch = jitter * (0.5f - rand() * 1.0f / RAND_MAX);
	jitterRot = jitter * (0.5f - rand() * 1.0f / RAND_MAX);
	jitter *= exp(-4.6 * sys->FACTOR);

	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), (float)deltaRot * sys->FACTOR + jitterRot, vUp);
	vRight = rot * glm::vec4(vRight, 1);
	vForward = rot * glm::vec4(vForward, 1);
	rot = glm::rotate(glm::mat4(1.0f), (float)deltaPitch * sys->FACTOR + jitterPitch, vRight);
	vUp = rot * glm::vec4(vUp, 1);
	vForward = rot * glm::vec4(vForward, 1);
	// ensure vectors are orthogonal
	vRight = glm::cross(vForward, vUp);
	vUp = glm::cross(vRight, vForward);
	// normalize vectors
	vForward = glm::normalize(vForward);
	vRight = glm::normalize(vRight);
	vUp = glm::normalize(vUp);

	collide(sys->planets);
	collide(sys->ships);
	collide(sys->asteroids);

	if (deltaRot > MAX_ANGULAR_VELOCITY)
		deltaRot = MAX_ANGULAR_VELOCITY;
	else if (deltaRot < -MAX_ANGULAR_VELOCITY)
		deltaRot = -MAX_ANGULAR_VELOCITY;
	if (deltaPitch > MAX_ANGULAR_VELOCITY)					// handle pitching motion
		deltaPitch = MAX_ANGULAR_VELOCITY;
	else if (deltaPitch < -MAX_ANGULAR_VELOCITY)
		deltaPitch = -MAX_ANGULAR_VELOCITY;

	if (abSpeed != 0) {
		if (glm::dot(velocity, velocity) > sqr(MAX_VELOCITY + abSpeed)) {
			float vel = sqrt(glm::dot(velocity, velocity));
			velocity *= (MAX_VELOCITY + abSpeed) / vel;
		}
	}
	else if (glm::dot(velocity, velocity) > (useThrottle ? sqr(throttledSpeed) : sqr(MAX_VELOCITY))) {
		float vel = sqrt(glm::dot(velocity, velocity));
		float speed;
		if (vel > (useThrottle ? throttledSpeed : MAX_VELOCITY) + ACCELERATION * sys->FACTOR)
			speed = vel - 1.1 * ACCELERATION * sys->FACTOR;
		else
			speed = useThrottle ? throttledSpeed : MAX_VELOCITY;
		velocity *= speed / vel;
	}
	else if (useThrottle && glm::dot(velocity, velocity) < sqr(throttledSpeed)) {
		goForward();
	}

	if (state == ALIVE) {
		shields += rechargeRate * sys->FACTOR;
		if (shields > maxShield)
			shields = maxShield;
	}
}

void Spaceship::bracket() {
	sys->shader->setUniform3f("ambient", 0, 1, 0);
	if( sys->enemies.count(this->shared_from_this()) )
		sys->shader->setUniform3f("ambient", 1, 0, 0);
	else if (escortee.get() == sys)
		sys->shader->setUniform3f("ambient", 1, 1, 0);
	if (state == DISABLED)
		sys->shader->setUniform3f("ambient", 0.5, 0.5, 0.5);
	SpaceObject::bracket();
}

void Spaceship::goLeft() {
	deltaRot += ANGULAR_ACCELERATION * sys->FACTOR;
	if( deltaRot < 0 )
		deltaRot += MAX_ANGULAR_VELOCITY * sys->FACTOR;
}

void Spaceship::goRight() {
	deltaRot -= ANGULAR_ACCELERATION * sys->FACTOR;
	if (deltaRot > 0)
		deltaRot -= MAX_ANGULAR_VELOCITY * sys->FACTOR;
}

void Spaceship::goForward() {
	velocity += vForward * (float)ACCELERATION * sys->FACTOR;
}

void Spaceship::goSlow() {
	throttledSpeed = 0;
}

void Spaceship::doAutopilot() {	// point at selection
	shared_ptr<SpaceObject> temp;

	if (selection)
		temp = static_pointer_cast<SpaceObject>(selection);
	else if (curPlanet)
		temp = static_pointer_cast<SpaceObject>(curPlanet);
	if (temp) {
		glm::vec3 local = globalToLocal(temp->pos);
		if (local.z < 0)
			left = true;
		else
			right = true;
		if( local.y > 0 )
			up = true;
		else
			down = true;
		if (sqr(local.x) / (sqr(local.x) + sqr(local.y) + sqr(local.z)) > 0.999f) {
			deltaPitch /= 2;
			deltaRot /= 2;
		}
	}
	else {
		autopilot = false;
	}
}

void Spaceship::doOrbit() {
	float distanceSquared = sqr(pos.x - centerOfRotation->pos.x) + sqr(pos.z - centerOfRotation->pos.z) + sqr(pos.y - centerOfRotation->pos.y);
	float a = sys->GRAVITY * centerOfRotation->mass / distanceSquared / 75;
	float _theta = atan2(centerOfRotation->pos.z - pos.z, centerOfRotation->pos.x - pos.x);
	float phi = atan2(centerOfRotation->pos.y - pos.y, sqrt(sqr(pos.x - centerOfRotation->pos.x) + sqr(pos.z - centerOfRotation->pos.z)));
	velocity.x += a * sys->FACTOR * cos(_theta) * cos(phi);
	velocity.z += a * sys->FACTOR * sin(_theta) * cos(phi);
	velocity.y += a * sys->FACTOR * sin(phi);
}

void Spaceship::toggleOrbit() {
	if (curPlanet && curPlanet != centerOfRotation)
		setOrbit(curPlanet);
	else
		centerOfRotation = nullptr;
}

void Spaceship::doHyperspace() {
	hyperTime += sys->FACTOR;
	if (hyperTime > 5.0f) {
		state = DEAD;
		if (sys->selection.get() == this) {
			sys->selection.reset();
			sys->shipIndex = -1;
		}
	}
	sys->enemies.erase(this->shared_from_this());
}

void Spaceship::doLand() {

}

void Spaceship::doFire(bool primary) {

}

void Spaceship::throttle(bool forward) {
	if (forward) {
		throttledSpeed += MAX_VELOCITY * sys->FACTOR * 0.333f;
		if (throttledSpeed > MAX_VELOCITY)
			throttledSpeed = MAX_VELOCITY;
	}
	else {
		throttledSpeed -= MAX_VELOCITY * sys->FACTOR * 0.333f;
		if (throttledSpeed < 0)
			throttledSpeed = 0;
	}
}

void Spaceship::doAfterburner() {

}

void Spaceship::goUp() {
	deltaPitch += ANGULAR_ACCELERATION * sys->FACTOR;
	if (deltaPitch < 0)
		deltaPitch += MAX_ANGULAR_VELOCITY * sys->FACTOR;
}

void Spaceship::goDown() {
	deltaPitch -= ANGULAR_ACCELERATION * sys->FACTOR;
	if (deltaPitch > 0)
		deltaPitch -= MAX_ANGULAR_VELOCITY * sys->FACTOR;
}

void Spaceship::addMod(shared_ptr<Mod> mod) {

}

bool Spaceship::addEnemy(shared_ptr<Spaceship> enemy) {
	if (!enemies.count(enemy)) {
		enemies.insert(enemy);
		return true;
	}
	return false;
}