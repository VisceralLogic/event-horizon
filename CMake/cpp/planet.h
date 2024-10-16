#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>

#include "spaceobject.h"
#include "object3D.h"
#include "ehobject.h"
#include "sphere.h"

class Solarsystem;

class Planet : public SpaceObject, public EHObject {
public:
	//NSMutableArray* goods;			// goods sold here
	bool shipyard, mod, mission;	// available amenities
	string shipFlags;				// flags required for this ship to show up
	string modFlags;				// ...
	string missionFlags;			// ...
	string texFile;					// texture file for dynamic loading
	shared_ptr<Object3D> model;				// used for a space station
	float inclination;				// orbital inclination from ecliptic
	string ringTex;				// path to optional ring texture file
	float ringSize;					// radius of ring
	string atmosTex;				// path to optional atmosphere texture file
	float atmosSize;				// ratio of radius of atmosphere to planetary radius
	float atmosSpeed;				// rotation of atmosphere (relative)
	float atmosRot;					// current relative angle of atmosphere
	float atmosAmbient[4];			// atmosphere ambient light
	shared_ptr<Solarsystem> system;			// which system this planet belongs to

	static Sphere* sphere;

	Planet();
	~Planet();

	static void initialize();
	static void registerFromDictionary(const json& dictionary);
	void finalize();
	void draw();
	void update();
	void orbit();

	static void setUpPlanet();
	static void drawPlanet();
	static void handleEvent(SDL_Event& event);
};