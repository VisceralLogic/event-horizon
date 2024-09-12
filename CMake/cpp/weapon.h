#pragma once

#include <memory>
#include <glad/glad.h>

#include "ehobject.h"
#include "object3D.h"

class Spaceship;
class Mod;

class Weapon : public Object3D, public EHObject {
public:
	shared_ptr<Spaceship> source;		// firing ship
	shared_ptr<Spaceship> target;		// target ship
	float life;							// how long this weapon lasts
	float reload;						// how long it takes to reload
	float lastShot;						// time since last shot
	bool isPrimary;						// is this a primary weapon
	int force;							// how much force shields/armor will be depleted
	GLuint modPic;						// texture for buying
	int count;							// how many of this weapon are on the ship
	int max;							// how many of this weapon can be on the ship
	float proximity;					// how close the target must be
	shared_ptr<Mod> launcher;			// launcher required by secondary weapon
	bool canFire;						// true if secondary and launcher present
	float inAcc;						// inaccuracy at start in degrees
	enum {
		STRAIGHT,
		TURRET,
		SPACESHIP,
		BEAM
	} launchStyle;
	int lastHardpoint;					// last hardpoint fired from
	shared_ptr<Spaceship> spaceship;	// for bay-launched escort ships
	int proxMass;						// mass for proximity explosion
	bool disableLighting;				// disable lighting for this weapon

	static void registerFromDictionary(const json& dictionary);
	void finalize();
};