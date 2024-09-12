#pragma once

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

using json = nlohmann::json;

class EHObject {
public:
	string ID;
	json initData;
	json flags;

	virtual void finalize() = 0;
};