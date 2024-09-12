#pragma once
#include <memory>
#include <map>
#include <string>

class Government;
class Mission;
class Mod;
class Planet;
class Spaceship;
class Solarsystem;
class Type;
class Weapon;

using namespace std;

class Plugin {
public:
	map<string, shared_ptr<Government>> governments;
	map<string, shared_ptr<Mission>> missions;
	map<string, shared_ptr<Mod>> mods;
	map<string, shared_ptr<Planet>> planets;
	map<string, shared_ptr<Spaceship>> spaceships;
	map<string, shared_ptr<Solarsystem>> solarsystems;
	map<string, shared_ptr<Type>> types;
	map<string, shared_ptr<Weapon>> weapons;

	static map<string, shared_ptr<Plugin>> plugins;
};