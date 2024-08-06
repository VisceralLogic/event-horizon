#pragma once
#include "planet.h"

class Government;
class Type;

class Solarsystem {
public:
	float x, z;
	string name;
	vector<Planet*> planets;
	vector<Solarsystem*> links;

	string ID;
/*
	NSMutableArray *initData;		// used to store data between init and finalize
	NSMutableDictionary *flags;		// generic flags
	*/
	int shipCount;					// average num of ships in system
	string description;			// description of this sytem for map
	int asteroids;					// number of asteroids in system
	Government *gov;				// which government this system belongs to
	Type *types;					// which ships should appear in this system
	int backdrops;					// number of backdrops, max of 4
	int backdropAngle[4];			// angle for each backdrop
	int backdropElev[4];			// elevation in pixels
	vector<string> backdropPath;		// path to backdrop graphic
	bool displayOnMap;				// show system info on map

	~Solarsystem();
	static void registerFromDictionary(map<void*, void*>& dictionary);
	void finalize();	// needed for plugins
	void setUp();		// the player just entered
	map<string, void*> flags();
	void update();

	vector<Planet*> getPlanets();
	vector<Solarsystem*> getLinks();
};