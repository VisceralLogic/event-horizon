#pragma once	

#include <set>
#include <memory>
#include <chrono>
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;

#include "spaceobject.h"
#include "planet.h"
#include "solarsystem.h"
#include "spaceship.h"
#include "asteroid.h"
#include "shader.h"
//#import "Background.h"

class Mission;
class Plugin;
class EHObject;

enum {
	NAVPANEL_TEXTURE,
	NAVPLANET_TEXTURE,
	WIDGET_TEXTURE,
	INFOTAB_TEXTURE,
	FIRE_TEXTURE,
	ASTEROID_TEXTURE,
	SHIELD_SPOT_TEXTURE,
	MENU_TEXTURE,
	NUM_TEXTURES
};

// keys (KeyMap codes)
enum {
	FORWARD,			// accelerate
	SLOW,				// decelerate
	RIGHT,				// rotate right
	LEFT,				// rotate left
	ORBIT,				// orbit selected planet
	AUTO,				// autopoint at selected ship or planet
	SELECT,				// select next planet
	SELECT_SHIP,		// select next ship
	SELECT_AT,			// select ship pointed at
	MAP,				// galactic map
	SELECT_SYSTEM,		// select next system
	HYPER,				// activate hyperspace jump
	REAR_VIEW,			// rear view
	RD_PERSON,			// 3rd person view
	TOP_VIEW,			// top view
	FIRE_PRIMARY,		// fire all primary weapons
	SELECT_SECONDARY,	// select next secondary weapon
	FIRE_SECONDARY,		// fire selected secondary weapon
	BOARD,				// board selected ship
	RECALL,				// recall/release escorts
	LAND,				// land on selected planet
	INVENTORY,			// display inventory
	PREF,				// display preferences
	PAUSE_KEY,			// pause game
	UP,					// move upward
	DOWN,				// move downward
	FULL,				// toggle fullscreen
	THROTTLE_UP,		// increase throttle setting
	THROTTLE_DOWN,		// decrease throttle setting
	CONSOLE,			// toggle console visibility
	AB,					// afterburner
	INERTIA,			// toggle inertial mode
	END_OF_KEYS
};

enum {					// pref float values
	MOUSE,				// 1.0 use mouse to control
	INVERT_Y,			// 1.0 invert the mouse y control
	SENSITIVITY,		// mouse sensitivity
	END_OF_FLOATS
};

class Controller : public Spaceship {
protected:
	static GLShaderProgram* stringShader;

public:
	static GLShaderProgram* shader;
	static GLShaderProgram* frameShader;

	vector<shared_ptr<Planet>> planets;
	shared_ptr<Solarsystem> system;
	string message;
	float messageTime;
	vector<shared_ptr<Spaceship>> ships;
	vector<shared_ptr<Weapon>> weaps;
	vector<shared_ptr<Asteroid>> asteroids;
	vector<shared_ptr<Mission>> missions;

	int planetIndex;
	int shipIndex;

	float FACTOR = 0;	// multiply by for varying FPS
	float GRAVITY;
	int screenWidth, screenHeight;
	float t;
	vector<GLuint> planetTex;

	Uint8* keys;					// key codes
	//Background* bg;					// draws stars
	int systemIndex;				// which system is selected
	vector<shared_ptr<Solarsystem>> itinerary;		// systems to travel to
	int viewStyle = 0;					// forward, top, back
	float shipCheckTime;			// when to check to see if new ships should be added to system
	float shipCheckDelta;			// how much to increase shipCheckTime
	chrono::system_clock::time_point date;					// game date
	bool paused = false;						// pause the game
	map<int, int> govRecord;	// key: gov ID, value: number reflecting position with regard to that government
	float floatVal[END_OF_FLOATS];		// pref values stored here
	int combatRating;				// combat experience rating
	//FSInterpreter* interpreter;		// use for FScripts
	bool console;					// is console visible
	GLuint frameVBO, frameVAO, frameEBO, squareVBO, squareVAO;	// frame buffers
	set<string> systems;		// visited systems

	static string basePath;
/*
+ (NSMutableDictionary *) ensurePlugin:(NSString *)pluginName path:(NSString *)path;
+ (NSMutableArray *) objectsOfType:(NSString *)type withFlags:(NSString *)flags;
+ (NSMutableArray *) objects:(NSArray *)objects withFlags:(NSString *)flags;
*/
	static void initialize();
	Controller(string name, bool isNew);
	void initPlugins();
	void initDataFor(const string& plugin);
	void loadFileForPlugin(const filesystem::path& file, const string& name);
	void finalizePlugin(string id);
	void update();
	void setSystem(shared_ptr<Solarsystem> system);
	void setShip(shared_ptr<Spaceship> ship);
	//- (NSMutableDictionary *) getPlugins;
	void save();
	void load();
	void doHyperspace();
	void doLand();
	bool functionKey();	// utility function to see if "map" or "inventory" key pressed"
	void pause();

	// graphics
	void setUpFrame();
	void drawNavTab();
	void drawFrame();
	void drawSelectionTab(int pos);
	void drawInfoTab();
	void drawObject(SpaceObject *o);
	glm::mat4& viewMatrix();

	static void drawString(const string& str, float x, float y, float *color);
	static vector<shared_ptr<EHObject>> objectsOfType(const string& type);
	static shared_ptr<EHObject> componentNamed(const string& name);
};

void drawGLScene();
vector<string> split(const string& str, char c);
string replace(const string& str, const string& find, const string& replace);
extern void (*drawScene)(void);
extern void (*eventScene)(SDL_Event&);
extern int gScreenWidth;
extern int gScreenHeight;
extern GLuint defaultFrameList;