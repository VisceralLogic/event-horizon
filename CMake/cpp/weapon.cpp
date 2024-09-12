#include <fstream>

#include "weapon.h"
#include "plugin.h"
#include "controller.h"
#include "texture.h"
#include "mod.h"

void Weapon::registerFromDictionary(const json& dictionary) {
	shared_ptr<Weapon> weap = make_shared<Weapon>();
	json empty;
	if( !dictionary.contains("Data") ) {
		return;
	}
	string dataFile = dictionary["Data"];
	if (dataFile.ends_with("obj") || dataFile.ends_with("OBJ")) {
		weap->loadOBJ((string)dictionary["Directory"] + dataFile);
	}
	else {
		ifstream fileStream(dataFile);
		if (!fileStream.is_open()) {
			return;
//			throw std::runtime_error("Could not open file: " + dataFile);
		}
		stringstream buffer;
		buffer << fileStream.rdbuf();
		weap->load(buffer.str().c_str(), (string)dictionary["Directory"] + (string)dictionary["PicturePath"]);
	}
	weap->name = dictionary.contains("Name") ? dictionary["Name"] : dictionary["ID"];
	weap->ID = (string)dictionary["PluginName"] + ".weapons." + (string)dictionary["ID"];
	weap->ANGULAR_ACCELERATION = dictionary.contains("AngularAcceleration") ? (float)(dictionary["AngularAcceleration"]) : 0;
	weap->MAX_ANGULAR_VELOCITY = dictionary.contains("MaxAngularVelocity") ? (float)(dictionary["MaxAngularVelocity"]) : 0;
	weap->ACCELERATION = dictionary.contains("Acceleration") ? (float)dictionary["Acceleration"] : 0;
	weap->MAX_VELOCITY = dictionary.contains("MaxSpeed") ? (float)dictionary["MaxSpeed"] : 0;
	weap->mass = dictionary.contains("Mass") ? (int)dictionary["Mass"] : 1;
	weap->size = dictionary.contains("Size") ? (float)dictionary["Size"] : 1.0f;
	weap->life = dictionary.contains("Life") ? (float)dictionary["Life"] : 1.0f;
	weap->isPrimary = dictionary.contains("IsPrimary") ? (bool)dictionary["IsPrimary"] : false;
	weap->flags = dictionary.contains("Flags") ? dictionary["Flags"] : empty;
	weap->description = dictionary.contains("Description") ? dictionary["Description"] : "";
	weap->price = dictionary.contains("Price") ? (int)dictionary["Price"] : 1000;
	weap->reload = dictionary.contains("Reload") ? (float)dictionary["Reload"] : 1.0f;
	weap->force = dictionary.contains("Force") ? (int)dictionary["Force"] : 10;
	weap->max = dictionary.contains("Max") ? (int)dictionary["Max"] : 0;
	weap->inAcc = dictionary.contains("Inaccuracy") ? (float)(dictionary["Inaccuracy"]) : 0;
	if( dictionary.contains("LaunchStyle") ) {
		weap->launchStyle = Weapon::STRAIGHT;
	}
	else {
		if (dictionary["LaunchStyle"] == "STRAIGHT")
			weap->launchStyle = Weapon::STRAIGHT;
		else if (dictionary["LaunchStyle"] == "TURRET")
			weap->launchStyle = Weapon::TURRET;
		else if (dictionary["LaunchStyle"] == "SPACESHIP")
			weap->launchStyle = Weapon::SPACESHIP;
		else if (dictionary["LaunchStyle"] == "BEAM")
			weap->launchStyle = Weapon::BEAM;
	}
	weap->proximity = dictionary.contains("Proximity") ? (float)(dictionary["Proximity"]) : 0;
	if( !weap->isPrimary )
		weap->initData["Launcher"] = dictionary.contains("Launcher") ? dictionary["Launcher"] : "";
	if( dictionary.contains("Spaceship") )
		weap->initData["Spaceship"] = dictionary["Spaceship"];
	glGenTextures(1, &weap->modPic);
	if( dictionary.contains("ModPic") )
		Texture::loadTexture((string)dictionary["Directory"] + (string)dictionary["ModPic"], &(weap->modPic), nullptr, nullptr);
	weap->flagRequirements = dictionary.contains("FlagRequirements") ? dictionary["FlagRequirements"] : "";
	weap->proxMass = dictionary.contains("ProxMass") ? (int)(dictionary["ProxMass"]) : 0;
	weap->disableLighting = dictionary.contains("DisableLighting") ? (bool)(dictionary["DisableLighting"]) : false;
	if (dictionary.contains("Replace"))
		weap->ID = dictionary["Replace"];
	vector<string> temp = split(weap->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->weapons[temp[2]] = weap;
}

void Weapon::finalize() {
	if( !isPrimary ) {
		launcher = static_pointer_cast<Mod>(Controller::componentNamed(initData["Launcher"]));
	}
	if( initData.contains("Spaceship") ) {
		spaceship = static_pointer_cast<Spaceship>(Controller::componentNamed(initData["Spaceship"]));
	}
	initData.clear();
}