#include "mod.h"
#include "controller.h"
#include "texture.h"
#include "plugin.h"
#include "weapon.h"

void Mod::registerFromDictionary(const json& dictionary) {
	shared_ptr<Mod> mod = make_shared<Mod>();
	json empty;
	mod->name = dictionary.contains("Name") ? (string)dictionary["Name"] : (string)dictionary["ID"];
	mod->ID = (string)dictionary["PluginName"] + ".mods." + (string)dictionary["ID"];
	mod->mass = dictionary.contains("Mass") ? (int)dictionary["Mass"] : 1;
	mod->flags = dictionary.contains("Flags") ? dictionary["Flags"] : empty;
	mod->description = dictionary.contains("Description") ? (string)dictionary["Description"] : "";
	mod->price = dictionary.contains("Price") ? (int)dictionary["Price"] : 1;
	mod->max = dictionary.contains("Max") ? (int)dictionary["Max"] : -1;
	if( dictionary.contains("Ammo") ) {
		mod->initData["Ammo"] = dictionary["Ammo"];
	}
	glGenTextures(1, &mod->modPic);
	string texPath = dictionary.contains("ModPicture") ? (string)dictionary["ModPicture"] : "";
	Texture::loadTexture((string)dictionary["Directory"] + texPath, &(mod->modPic), nullptr, nullptr);
	mod->value = dictionary.contains("Value") ? (float)dictionary["Value"] : 0;
	if( !dictionary.contains("ModType") ) {
		mod->modType = Mod::AFTERBURNER;
	}
	else {
		if (dictionary["ModType"] == "LAUNCHER")
		{
			mod->modType = Mod::LAUNCHER;
		}
		else if (dictionary["ModType"] == "COMPENSATOR") {
			mod->modType = Mod::COMPENSATOR;
		}
		else if (dictionary["ModType"] == "FUELTANK") {
			mod->modType = Mod::FUELTANK;
		}
		else if (dictionary["ModType"] == "AFTERBURNER") {
			mod->modType = Mod::AFTERBURNER;
		}
	}
	mod->value2 = dictionary.contains("Value2") ? (float)dictionary["Value2"] : 0;
	mod->value3 = dictionary.contains("Value3") ? (float)dictionary["Value3"] : 0;
	if (dictionary.contains("Replace"))
		mod->ID = dictionary["Replace"];
	vector<string> temp = split(mod->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->mods[temp[2]] = mod;
}

void Mod::finalize() {
	if( initData.contains("Ammo") ) {
		ammo = static_pointer_cast<Weapon>(Controller::componentNamed(initData["Ammo"]));
	}
	initData.clear();
}

shared_ptr<Mod> Mod::shipInstance() {
	shared_ptr<Mod> mod = make_shared<Mod>();
	mod->name = name;
	mod->ID = ID;
	mod->mass = mass;
	mod->flags = flags;
	mod->max = max;
	mod->value = value;
	mod->modType = modType;
	mod->value2 = value2;
	mod->value3 = value3;
	mod->ammo = ammo;
	mod->count = 1;
	return mod;
}