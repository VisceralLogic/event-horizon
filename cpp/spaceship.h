#pragma once
#include <set>

#include "object3D.h"
#include "coord.h"
//#include "government.h"

class AI;
class Weapon;
class Mod;
class Cargo;
class Government;

class Spaceship : public Object3D {
public:
	bool left, right, forward, slow, autopilot, orbit, hyperspace, land, fire, fireSecondary, up, down, throttleUp, throttleDown, afterburner;
	Planet* curPlanet;				// selected planet
	AI* selection;					// selected spaceship
	float hyperTime;				// powering up for hyper jump
	int cargoSpace;					// max room for cargo
	int modSpace;					// room for mods;
	vector<Mod*> mods;			// array of mods
	vector<Cargo*> cargo;			// array of current cargos
	int money;						// how much money the ship has
	vector<Weapon*> weapons;		// array of weapons
	int secondaryIndex;				// which secondary weapon is selected
	float shields;					// 
	int armor;						//
	float fuel;						//
	float thrust;					// divided by mass to find accel
	int maxShield;					// used to calculate percentages and regenerate
	int maxArmor;					//
	int maxFuel;					//
	string defaultAI;			// which AI to use
	set<Spaceship*> enemies;		// enemy ships in system
	Government* gov;				// which government this ship belongs to
	enum {
		ALIVE,
		DISABLED,
		DEAD
	} state;						// what state is this ship currently in
	vector<Weapon*> secondary;		// array of secondary weapon launchers
	vector<Coord> primaryStraight, primaryTurret, secondaryStraight, secondaryTurret;
	// array of hardpoints to fire weapons from
	Spaceship* escortee;			// spaceship to escort
	vector<AI*> escorts;		// array of escorting ships
	string weaponID;				// ID of weapon, if bay-launched
	bool bound;						// strict escortee control
	float compensation;				// amount of inertial compensation
	bool useThrottle;				// is the throttle engaged?
	float throttledSpeed;			// the desired speed
	float rechargeRate;				// shield recharge rate, units/sec
	float abSpeed;					// additional speed from A/B
	vector<Coord> shieldSpots;		// array of places shield hit
	float jitter;					// random jitter due to being hit
	bool inertial;					// if true, don't auto-brake
	string frameFile;			// file containing frame definition

	Spaceship();
	~Spaceship();
	static void registerFromDictionary(map<void*, void*>& dictionary);
	void finalize();	// needed for plugins
	virtual void update();
	void goLeft();
	void goRight();
	void goForward();
	void goSlow();
	void goUp();
	void goDown();
	void doAutopilot();
	void doOrbit();
	void toggleOrbit();
	virtual void doHyperspace();
	virtual void doLand();
	void doFire(bool primary);
	Spaceship* newInstance();
	void addMod(Mod* mod);
	Mod* hasMod();
	void hitBy(Weapon* weapon);
	bool addEnemy(Spaceship* enemy);
	void bracket();
	void throttle(bool forward);
	void doAfterburner();
};