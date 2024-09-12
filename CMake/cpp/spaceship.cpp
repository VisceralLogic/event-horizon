#include <iostream>
#include <fstream>
#include <sstream>

#include "spaceship.h"
#include "controller.h"
#include "plugin.h"

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

void Spaceship::registerFromDictionary(const json& dictionary) {
	shared_ptr<Spaceship> ship = make_shared<Spaceship>();
	json empty;
	if (!dictionary.contains("Data"))
		return;
	string dataFile = dictionary["Data"];
	if (dataFile.ends_with("obj") || dataFile.ends_with("OBJ")) {
		ship->loadOBJ((string)dictionary["Directory"] + dataFile);
	}
	else {
		ifstream fileStream(dataFile);
		if (!fileStream.is_open()) {
			//throw std::runtime_error("Could not open file: " + dataFile);
			return;
		}
		stringstream buffer;
		buffer << fileStream.rdbuf();
		ship->load(buffer.str().c_str(), (string)dictionary["Directory"] + (string)dictionary["PicturePath"]);
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
	if( sys->enemies.count(shared_ptr<Spaceship>(this)) )
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