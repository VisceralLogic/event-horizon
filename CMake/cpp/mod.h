#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <glad/glad.h>

#include "ehobject.h"
#include "spaceobject.h"

using namespace std;

using json = nlohmann::json;

class Weapon;

class Mod : public EHObject {
public:
	int mass;					// how much room this mod takes up
	int price;					// how much this mod costs
	GLuint modPic;				// texture for buying
	string name;				// name of the mod
	json flags;					// various flags
	string description;			// description of the mod
	int count;					// how many of this mod are on the ship
	int max;					// how many of this mod can be on the ship
	shared_ptr<Weapon> ammo;	// weapon this mod is attached to
	json initData;				// data from the dictionary
	string flagRequirements;	// needed on planet to be sold there
	enum {
		LAUNCHER,				// secondary weapon launcher
		COMPENSATOR,			// inertial compensator
		FUELTANK,				// fuel tank
		AFTERBURNER,			// activated mod, adds thrust
	} modType;
	float value, value2, value3;	// various values

	static void registerFromDictionary(const json& dictionary);
	void finalize();
	shared_ptr<Mod> shipInstance();
};