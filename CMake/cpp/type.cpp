#include "type.h"
#include "controller.h"
#include "plugin.h"
#include "ai.h"
#include "government.h"

void Type::registerFromDictionary(const json& dictionary) {
	shared_ptr<Type> type = make_shared<Type>();
	type->initData = dictionary["Data"];
	type->ID = (string)dictionary["PluginName"] + ".types." + (string)dictionary["ID"];
	if (dictionary.contains("Replace"))
		type->ID = dictionary["Replace"];
	vector<string> temp = split(type->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->types[temp[2]] = type;
	if( dictionary.contains("ADDTO") )
		type->ID = "ADDTO" + dictionary["ADDTO"];
}

void Type::finalize() {
	if( this->ID.find("ADDTO") == 0 ) {
		shared_ptr<Type> type = static_pointer_cast<Type>(Controller::componentNamed(this->ID.substr(5)));
		float f = 0;
		for( auto& data : initData ) {
			f += data["Probability"];
		}
		if (f > 1) {
			f = 1;
		}
		for (auto& data : type->initData) {
			data["Probability"] = data["Probability"] * (1.0f - f);
		}
		for (auto& data : initData) {
			type->initData.push_back(data);
		}
	}
}

shared_ptr<Spaceship> Type::newInstance() {
	float prob = (float)rand() / RAND_MAX;

	for( auto& data : initData ) {
		prob -= (float)data["Probability"];
		if( prob <= 0 ) {
			if (data.contains("ShipID")) {
				shared_ptr<Spaceship> ship = static_pointer_cast<Spaceship>(Controller::componentNamed(data["ShipID"]))->newInstance();
				ship->gov = static_pointer_cast<Government>(Controller::componentNamed(data["GovernmentID"]));
				return ship;
			}
			else if (data.contains("TypeID")) {
				shared_ptr<Type> type = static_pointer_cast<Type>(Controller::componentNamed(data["TypeID"]));
				return type->newInstance();
			}
		}
	}

	return nullptr;
}