#pragma once	

#include <set>
#include <SDL2/SDL.h>

#include "spaceobject.h"
#include "planet.h"
#include "solarsystem.h"
#include "spaceship.h"
#include "asteroid.h"
#include "shader.h"
//#import "Background.h"

class Mission;
class Plugin;

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
	vector<Planet*> planets;
	Solarsystem* system;
	string message;
	float messageTime;
	vector<Spaceship*> ships;
	vector<Weapon*> weaps;
	vector<Asteroid*> asteroids;
	vector<Mission*> missions;

	int planetIndex;
	int shipIndex;

	float FACTOR;	// multiply by for varying FPS
	map<string, Plugin*> plugins;
	float GRAVITY;
	int screenWidth, screenHeight;
	float t;
	GLuint planetTex[16];

	int index[END_OF_KEYS];
	int val[END_OF_KEYS];
	//Background* bg;					// draws stars
	int systemIndex;				// which system is selected
	vector<Solarsystem*> itinerary;		// systems to travel to
	int viewStyle;					// forward, top, back
	float shipCheckTime;			// when to check to see if new ships should be added to system
	float shipCheckDelta;			// how much to increase shipCheckTime
	//NSDate* date;					// game date
	bool pause;						// pause the game
	map<int, int> govRecord;	// key: gov ID, value: number reflecting position with regard to that government
	float floatVal[END_OF_FLOATS];		// pref values stored here
	int combatRating;				// combat experience rating
	//FSInterpreter* interpreter;		// use for FScripts
	bool console;					// is console visible
	GLuint frameFront, frameRear;	// frame display lists
	set<string> systems;		// visited systems

	static string basePath;

	static void* componentNamed(string name);

/*
+ (NSMutableDictionary *) ensurePlugin:(NSString *)pluginName path:(NSString *)path;
+ (NSMutableArray *) objectsOfType:(NSString *)type withFlags:(NSString *)flags;
+ (NSMutableArray *) objects:(NSArray *)objects withFlags:(NSString *)flags;
+ (NSMutableArray *) objectsOfType:(NSString *)type;
*/
	static void initialize();
	Controller(string name, bool isNew);
	void initPlugins();
	//void initDataFor(Plugin* plugin);
	void loadFileForPlugin(string file, string name);
	void finalizePlugin(string id);
	void update();
	void setSystem(Solarsystem* system);
	void setShip(Spaceship* ship);
	//- (NSMutableDictionary *) getPlugins;
	void save();
	void load();
	void doHyperspace();
	void doLand();
	bool functionKey();	// utility function to see if "map" or "inventory" key pressed"

	// graphics
	void setUpFrame();
	void drawNavTab();
	void drawFrame();
	void drawSelectionTab(int pos);
	void drawInfoTab();

	static void drawString(const string& str, float x, float y, float *color);
};

void drawGLScene();
void beginOrtho();
void endOrtho();
void drawString(string str, float x, float y);
extern void (*drawScene)(void);
extern void (*eventScene)(SDL_Event&);
extern int gScreenWidth;
extern int gScreenHeight;
extern GLuint defaultFrameList;