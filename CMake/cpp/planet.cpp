#include "planet.h"
#include "controller.h"
#include "plugin.h"

Sphere* Planet::planetSphere;

void Planet::initialize() {
	planetSphere = new Sphere(20, 20);
	// load sounds
}

void Planet::registerFromDictionary(const json& dictionary) {
	shared_ptr<Planet> planet = make_shared<Planet>();
	json empty;
	if( dictionary.contains("Coords") && dictionary["Coords"].is_array() ) {
		planet->pos.x = dictionary["Coords"][0];
		planet->pos.y = dictionary["Coords"][1];
	}
	planet->name = dictionary.contains("Name") ? (string)dictionary["Name"] : (string)dictionary["ID"];
	planet->ID = (string)dictionary["PluginName"] + ".planets." + (string)dictionary["ID"];
	planet->size = dictionary.contains("Size") ? (float)dictionary["Size"] : 1;
	planet->deltaRot = dictionary.contains("Speed") ? (float)dictionary["Speed"] : 0;
	planet->mass = dictionary.contains("Mass") ? (int)dictionary["Mass"] : 1;
	planet->initData = dictionary.contains("Orbit") ? dictionary["Orbit"] : empty;
	planet->description = dictionary.contains("Description") ? (string)dictionary["Description"] : "";
	// goods
	if( dictionary.contains("PicturePath") ) {
		planet->texFile = (string)dictionary["Directory"] + (string)dictionary["PicturePath"];
		planet->texNum = 1;
		planet->texture.push_back(0);
	} else if( dictionary.contains("Model") ) {
		planet->model = make_shared<Object3D>();
		planet->model->loadOBJ((string)dictionary["Directory"] + (string)dictionary["Model"]);
	}
	planet->flags = dictionary.contains("Flags") ? dictionary["Flags"] : empty;
	if( dictionary.contains("ShipFlags") ) {
		planet->shipFlags = dictionary["ShipFlags"];
		planet->shipyard = true;
	}
	if( dictionary.contains("ModFlags") ) {
		planet->modFlags = dictionary["ModFlags"];
		planet->mod = true;
	}
	if( dictionary.contains("MissionFlags") ) {
		planet->missionFlags = dictionary["MissionFlags"];
		planet->mission = true;
	}
	planet->pitch = dictionary.contains("Tilt") ? (float)dictionary["Tilt"] : 0;
	planet->inclination = dictionary.contains("Inclination") ? (float)dictionary["Inclination"]*pi/180 : 0;
	if( dictionary.contains("Ring") ) {
		planet->ringTex = (string)dictionary["Directory"] + (string)dictionary["Ring"]["Path"];
		planet->ringSize = dictionary["Ring"]["Size"];
	}
	if (dictionary.contains("Atmosphere")) {
		planet->atmosTex = (string)dictionary["Directory"] + (string)dictionary["Atmosphere"]["Path"];
		planet->atmosSize = dictionary["Atmosphere"]["Size"];
		planet->atmosSpeed = dictionary["Atmosphere"]["Speed"];
	}
	if( dictionary.contains("Replace") )
		planet->ID = dictionary["Replace"];
	vector<string> temp = split(planet->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->planets[temp[2]] = planet;
}

void Planet::finalize() {
	if (initData.is_null())
		return;
	// set up orbit
	if( initData.is_string() )
		setOrbit(static_pointer_cast<Planet>(Controller::componentNamed(initData)));
	else if (initData.is_number()) {
		SPACEOBJECT_ORIGIN->mass = initData;
		setOrbit(SPACEOBJECT_ORIGIN);
	}
	initData.clear();
}

Planet::Planet() {
	texture.resize(3);
	texNum = 3;
}

Planet::~Planet() {
	for( int i = 0; i < texNum; i++ ) {
		glDeleteTextures(1, &texture[i]);
	}
}