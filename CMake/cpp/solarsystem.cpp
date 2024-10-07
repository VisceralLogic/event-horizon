#include "solarsystem.h"
#include "spaceobject.h"
#include "controller.h"
#include "texture.h"
#include "ai.h"
#include "plugin.h"
#include "type.h"
#include "government.h"
#include "background.h"

GLuint backdrop[4];
int backdropWidth[4];
int backdropHeight[4];

void Solarsystem::setUp() {
	int i;

	displayOnMap = true;
	sys->selection = nullptr;
	sys->shipIndex = -1;
	sys->curPlanet = nullptr;
	sys->planetIndex = -1;
	sys->enemies.clear();
	sys->ships.clear();
	sys->ships.push_back(shared_ptr<Spaceship>(sys));
	sys->weaps.clear();
	sys->asteroids.clear();
	for( GLuint& tex : sys->planetTex ) {
		glDeleteTextures(1, &tex);
	}
	sys->planetTex.clear();
	for (i = 0; i < planets.size(); i++) {
		shared_ptr<Planet> planet = planets[i];
		if (!planet->model) {
			sys->planetTex.push_back(0);
			Texture::loadTexture(planet->texFile.c_str(), &sys->planetTex.back(), nullptr, nullptr);
			planet->texture[0] = sys->planetTex.back();
			if (planet->ringSize != 0) {
				sys->planetTex.push_back(0);
				Texture::loadTexture(planet->ringTex.c_str(), &sys->planetTex.back(), nullptr, nullptr);
				planet->texture[1] = sys->planetTex.back();
			}
			if (planet->atmosSize != 0) {
				sys->planetTex.push_back(0);
				Texture::loadTexture(planet->atmosTex.c_str(), &sys->planetTex.back(), nullptr, nullptr);
				planet->texture[2] = sys->planetTex.back();
			}
		}
	}
	sys->planets = planets;
	for (i = 0; i < shipCount; i++) {
		shared_ptr<AI> tempShip = types->newInstance();
		tempShip->pos.x = 60.0f - (120.0f * rand()) / RAND_MAX;
		tempShip->pos.z = 60.0f - (120.0f * rand()) / RAND_MAX;
		tempShip->pos.y = 60.0f - (120.0f * rand()) / RAND_MAX;
		sys->ships.push_back(tempShip);
	}
	for (i = 0; i < asteroids; i++) {
		sys->asteroids.push_back(shared_ptr<Asteroid>(new Asteroid()));
	}
	for (i = 0; i < backdrops; i++) {
		Texture::loadTexture(backdropPath[i].c_str(), &backdrop[i], &backdropWidth[i], &backdropHeight[i]);
	}
	if (!sys->systems.count(ID))
		sys->systems.emplace(ID);
	Background::generate(0.2f);
}

void Solarsystem::registerFromDictionary(const json& dictionary) {
	shared_ptr<Solarsystem> system = make_shared<Solarsystem>();
	json empty;
	system->name = dictionary.contains("Name") ? dictionary["Name"] : dictionary["ID"];
	system->ID = (string)dictionary["PluginName"] + ".systems." + (string)dictionary["ID"];
	if( dictionary.contains("Coords") && dictionary["Coords"].is_array() ) {
		system->x = dictionary["Coords"][0];
		system->z = dictionary["Coords"][1];
	}
	else {
		system->x = 0;
		system->z = 0;
	}
	if (dictionary.contains("Start")) {
		Plugin::plugins["default"]->solarsystems["start"] = system;
	}
	system->initData["Planets"] = dictionary.contains("Planets") ? dictionary["Planets"] : empty;
	system->initData["Links"] = dictionary.contains("Links") ? dictionary["Links"] : empty;
	system->initData["Government"] = dictionary["Government"];
	system->flags = dictionary.contains("Flags") ? dictionary["Flags"] : empty;
	system->shipCount = dictionary.contains("ShipCount") ? (int)dictionary["ShipCount"] : 1;
	system->description = dictionary.contains("Description") ? (string)dictionary["Description"] : "";
	system->asteroids = dictionary.contains("Asteroids") ? (int)dictionary["Asteroids"] : 5;
	system->initData["ShipTypes"] = dictionary.contains("ShipTypes") ? dictionary["ShipTypes"] : empty;
	if (dictionary.contains("Backdrops") && dictionary["Backdrops"].is_array()) {
		system->backdrops = dictionary["Backdrops"].size();
		for (int i = 0; i < system->backdrops; i++) {
			system->backdropPath.push_back((string)dictionary["Directory"] + (string)dictionary["Backdrops"][i]["Path"]);
			system->backdropAngle[i] = dictionary["Backdrops"][i]["Angle"];
			system->backdropElev[i] = dictionary["Backdrops"][i]["Height"];
		}
	}
	else {
		system->backdrops = 0;
	}
	system->displayOnMap = false;
	if (dictionary.contains("Replace"))
		system->ID = dictionary["Replace"];
	vector<string> temp = split(system->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->solarsystems[temp[2]] = system;
}

void Solarsystem::finalize() {
	if (initData["Planets"].is_array()) {
		for (auto& planet : initData["Planets"]) {
			planets.push_back(static_pointer_cast<Planet>(Controller::componentNamed(planet)));
		}
	}

	if (initData["Links"].is_array()) {
		for (auto& link : initData["Links"]) {
			shared_ptr<Solarsystem> s = static_pointer_cast<Solarsystem>(Controller::componentNamed(link));
			if (find(links.begin(), links.end(), s) == links.end())
				links.push_back(s);
			if (find(s->links.begin(), s->links.end(), shared_from_this()) == s->links.end())
				s->links.push_back(shared_from_this());
		}
	}

	if( !initData["Government"].is_null() ) {
		gov = static_pointer_cast<Government>(Controller::componentNamed(initData["Government"]));
	}

	types = make_shared<Type>();
	types->initData = initData["ShipTypes"];

	//initData.clear();
}

void Solarsystem::update() {
	int i;

	if (sys->viewStyle == 1)
		return;
	/*glDisable(GL_DEPTH_TEST);
	glColor3f(1, 1, 1);
	glLoadIdentity();
	if (sys->viewStyle == 0) {
		glRotatef(-sys->pitch, 1, 0, 0);
		glRotatef(-sys->roll, 0, 0, 1);
		glRotatef(180 - sys->angle, 0, 1, 0);
	}
	else if (sys->viewStyle == 2) {
		glRotatef(sys->pitch, 1, 0, 0);
		glRotatef(sys->roll, 0, 0, 1);
		glRotatef(90 - sys->angle, 0, 1, 0);
	}
	else if (sys->viewStyle == 3) {
		glRotatef(sys->deltaRot / 10, 0, 1, 0);
		glRotatef(sys->deltaPitch / 10, 1, 0, 0);
		glRotatef(-sys->pitch, 1, 0, 0);
		glRotatef(-sys->roll, 0, 0, 1);
		glRotatef(270 - sys->angle, 0, 1, 0);
	}
	for (i = 0; i < backdrops; i++) {
		float xSize = backdropWidth[i] / 4.0f;
		float ySize = backdropHeight[i] / 4.0f;
		float back = sqrt(250000 - xSize * xSize - ySize * ySize) - 50;
		glBindTexture(GL_TEXTURE_2D, backdrop[i]);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3f(-xSize, -ySize, -back);
		glTexCoord2d(1, 0);
		glVertex3f(xSize, -ySize, -back);
		glTexCoord2d(1, 1);
		glVertex3f(xSize, ySize, -back);
		glTexCoord2d(0, 1);
		glVertex3f(-xSize, ySize, -back);
		glEnd();
	}
	glEnable(GL_DEPTH_TEST);*/
}

Solarsystem::~Solarsystem() {
}