#pragma once
#include <set>
#include <memory>

#include "object3D.h"
#include "coord.h"
#include "ehobject.h"
//#include "government.h"

class AI;
class Weapon;
class Mod;
class Cargo;
class Government;

class Spaceship : public Object3D, public EHObject, public std::enable_shared_from_this<Spaceship> {
public:
	bool left = false, right = false, forward, slow, autopilot, orbit, hyperspace, land, fire, fireSecondary, up, down, throttleUp, throttleDown, afterburner;
	shared_ptr<Planet> curPlanet;				// selected planet
	shared_ptr<AI> selection;					// selected spaceship
	float hyperTime;				// powering up for hyper jump
	int cargoSpace;					// max room for cargo
	int modSpace;					// room for mods;
	vector<shared_ptr<Mod>> mods;			// array of mods
	vector<Cargo> cargo;			// array of current cargos
	int money;						// how much money the ship has
	vector<shared_ptr<Weapon>> weapons;		// array of weapons
	int secondaryIndex;				// which secondary weapon is selected
	float shields;					// 
	int armor;						//
	float fuel;						//
	float thrust;					// divided by mass to find accel
	int maxShield;					// used to calculate percentages and regenerate
	int maxArmor;					//
	int maxFuel;					//
	string defaultAI;			// which AI to use
	set<shared_ptr<Spaceship>> enemies;		// enemy ships in system
	shared_ptr<Government> gov;				// which government this ship belongs to
	enum {
		ALIVE,
		DISABLED,
		DEAD
	} state;						// what state is this ship currently in
	vector<shared_ptr<Weapon>> secondary;		// array of secondary weapon launchers
	vector<Coord> primaryStraight, primaryTurret, secondaryStraight, secondaryTurret;
	// array of hardpoints to fire weapons from
	shared_ptr<Spaceship> escortee;			// spaceship to escort
	vector<shared_ptr<AI>> escorts;		// array of escorting ships
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
	static void registerFromDictionary(const json& dictionary);
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
	shared_ptr<Spaceship> newInstance();
	void addMod(shared_ptr<Mod> mod);
	Mod* hasMod();
	void hitBy(shared_ptr<Weapon> weapon);
	bool addEnemy(shared_ptr<Spaceship> enemy);
	void bracket();
	void throttle(bool forward);
	void doAfterburner();

	static const float jumpDistance;
};