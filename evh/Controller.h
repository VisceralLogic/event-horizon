//
//  Controller.h
//  Event Horizon
//
//  Created by user on Thu Aug 08 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "SpaceObject.h"
#import "Planet.h"
#import "Solarsystem.h"
#import "Spaceship.h"
#import "AI.h"
#import "Background.h"
#import <FScript/FScript.h>

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

enum{					// pref float values
	MOUSE,				// 1.0 use mouse to control
	INVERT_Y,			// 1.0 invert the mouse y control
	SENSITIVITY,		// mouse sensitivity
	END_OF_FLOATS
};

@interface Controller : Spaceship {
@public
	NSMutableArray *planets;		// this system's planets
	Solarsystem *system;					// this system
	NSString *message;				// user status message
	float messageTime;				// how long has message been showing?
	NSMutableArray *ships;			// ships currently in system
	NSMutableArray *weaps;			// weapons in system
	NSMutableArray *asteroids;		// array of current asteroids
	NSMutableArray *missions;		// current missions undertaken by player
	
	int planetIndex;				// index to current planet
	int shipIndex;					// index to current ship

	float FACTOR;					// multiply by for varying fps
	NSMutableDictionary *plugins;	// dictionary of all plugins
	float GRAVITY;
	int screenWidth;
	int screenHeight;
	float t;
	GLuint planetTex[16];			// array of planet texture pointers

	int index[END_OF_KEYS];
	int val[END_OF_KEYS];
	Background *bg;					// draws stars
	int systemIndex;				// which system is selected
	NSMutableArray *itinerary;		// systems to travel to
	int viewStyle;					// forward, top, back
	float shipCheckTime;			// when to check to see if new ships should be added to system
	float shipCheckDelta;			// how much to increase shipCheckTime
	NSDate *date;					// game date
	BOOL pause;						// pause the game
	NSMutableDictionary *govRecord;	// key: gov ID, value: number reflecting position with regard to that government
	float floatVal[END_OF_FLOATS];		// pref values stored here
	int combatRating;				// combat experience rating
	FSInterpreter *interpreter;		// use for FScripts
	BOOL console;					// is console visible
	GLuint frameFront, frameRear;	// frame display lists
	NSMutableArray *systems;		// visited systems
}

-		 initWithName:(NSString*)name new:(BOOL)new;
- (void) initPlugins;
- (void) initDataFor:(NSBundle *)plugin;
- (void) loadFile:(NSString *)file forPlugin:(NSString *)name;
- (void) finalizePlugin:(id)plugin;
- (void) update;
- (void) setSystem:(Solarsystem *)system;
- (int) setShip:(Spaceship *)ship;
- (NSMutableDictionary *) getPlugins;
+ componentNamed:(NSString *)name;
+ (NSMutableDictionary *) ensurePlugin:(NSString *)pluginName path:(NSString *)path;
+ (NSMutableArray *) objectsOfType:(NSString *)type withFlags:(NSString *)flags;
+ (NSMutableArray *) objects:(NSArray *)objects withFlags:(NSString *)flags;
+ (NSMutableArray *) objectsOfType:(NSString *)type;
- (void) save;
- (void) load;
- (void) hyper;					// these functions replace generic Spaceship functions
- (void) land;
- (BOOL) functionKey;			// utility function to see if "map" or "inventory" key pressed

@end