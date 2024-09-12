#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include <chrono>

#include "ehobject.h"

using namespace std;

using json = nlohmann::json;

class Solarsystem;
class Planet;

class Mission : public EHObject {
public:
	string availType;	// affects how 'avail' is interpreted ['government'|'system'|'planet']
	string avail;
	string availFlags;	// what flags must be set
	string destType;	// same as availType
	string dest;
	shared_ptr<Solarsystem> startSystem;	// initialized in newInstanceIn
	shared_ptr<Planet> startPlanet;			// ...
	shared_ptr<Solarsystem> endSystem;		// ...
	shared_ptr<Planet> endPlanet;			// ...
	string startFlags;		// which flags to set upon starting
	string setFlags;		// which flags to set upon completion
	int size;				// how much cargo
	int payment;			// how much money
	string title;			// name of the mission
	string description;		// description of the mission
	float freq;				// how often this mission is available (0-1)
	//NSMutableDictionary* flags;
	string endText;			// text to display upon completion
	string failText;		// text to display upon failure
	int endTime;			// how many days before the mission expires
	chrono::system_clock::time_point expires;			// when the mission expires

	static void registerFromDictionary(const json& dictionary);
	void finalize();
	/*-(Mission*)suitableSystem:(Solarsystem*)system andPlanet : (Planet*)planet;
	-(NSDictionary*)flags;*/
	shared_ptr<Mission> newInstanceIn(shared_ptr<Solarsystem> system, shared_ptr<Planet> planet);
	void accept();
	void accomplish();
	void update();
};