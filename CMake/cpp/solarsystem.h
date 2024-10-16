#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "planet.h"
#include "ehobject.h"

class Government;
class Type;

using json = nlohmann::json;

class Solarsystem : public EHObject, public enable_shared_from_this<Solarsystem> {
public:
	float x, z;
	string name;
	vector<shared_ptr<Planet>> planets;
	vector<shared_ptr<Solarsystem>> links;

	int shipCount;					// average num of ships in system
	string description;			// description of this sytem for map
	int asteroids;					// number of asteroids in system
	shared_ptr<Government> gov;				// which government this system belongs to
	shared_ptr<Type> types;					// which ships should appear in this system
	int backdrops;					// number of backdrops, max of 4
	int backdropAngle[4];			// angle for each backdrop
	int backdropElev[4];			// elevation in pixels
	vector<string> backdropPath;		// path to backdrop graphic
	bool displayOnMap;				// show system info on map
	glm::vec3 lightDir;				// direction of light source
	glm::vec3 ambient;				// ambient light

	~Solarsystem();
	static void registerFromDictionary(const json& dictionary);
	void finalize();	// needed for plugins
	void setUp();		// the player just entered
	void update();

	vector<shared_ptr<Planet>> getPlanets();
	vector<shared_ptr<Solarsystem>> getLinks();
};