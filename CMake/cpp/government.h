#pragma once

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <nlohmann/json.hpp>

#include "ehobject.h"

using namespace std;
using json = nlohmann::json;

class Government : public EHObject {
protected:
	json initData;

public:
	string name;
	set<string> enemies;
	set<string> allies;
	float red, green, blue;
	int shootPenalty, disablePenalty, destroyPenalty, boardPenalty;

	static void registerFromDictionary(const json& dictionary);
	void finalize();
};