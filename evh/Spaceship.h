//
//  Spaceship.h
//  Event Horizon
//
//  Created by user on Thu Dec 26 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "Object3D.h"
#import "Coord.h"
#import "Government.h"

@class AI;
@class Weapon;

#define jumpDistance 10000

@interface Spaceship : Object3D {
@public
	BOOL left, right, forward, slow, autopilot, orbit, hyper, land, fire, fireSecondary, up, down, throttleUp, throttleDown, afterburner;
	Planet *curPlanet;				// selected planet
	AI *selection;					// selected spaceship
	float hyperTime;				// powering up for hyper jump
	int cargoSpace;					// max room for cargo
	int modSpace;					// room for mods;
	NSMutableArray *mods;			// array of mods
	NSMutableArray *cargo;			// array of current cargos
	int money;						// how much money the ship has
	NSMutableArray *weapons;		// array of weapons
	int secondaryIndex;				// which secondary weapon is selected
	float shields;					// 
	int armor;						//
	float fuel;						//
	float thrust;					// divided by mass to find accel
	int maxShield;					// used to calculate percentages and regenerate
	int maxArmor;					//
	int maxFuel;					//
	NSString *defaultAI;			// which AI to use
	NSMutableArray *enemies;		// enemy ships in system
	Government *gov;				// which government this ship belongs to
	enum{
		ALIVE,
		DISABLED,
		DEAD
	} state;						// what state is this ship currently in
	NSMutableArray *secondary;		// array of secondary weapon launchers
	NSMutableArray *primaryStraight, *primaryTurret, *secondaryStraight, *secondaryTurret;
		// array of hardpoints to fire weapons from
	Spaceship *escortee;			// spaceship to escort
	NSMutableArray *escorts;		// array of escorting ships
	NSString *weaponID;				// ID of weapon, if bay-launched
	BOOL bound;						// strict escortee control
	float compensation;				// amount of inertial compensation
	BOOL throttle;					// is the throttle engaged?
	float throttledSpeed;			// the desired speed
	float rechargeRate;				// shield recharge rate, units/sec
	float abSpeed;					// additional speed from A/B
	NSMutableArray *shieldSpots;	// array of places shield hit
	float jitter;					// random jitter due to being hit
	BOOL inertial;					// if true, don't auto-brake
	NSString *frameFile;			// file containing frame definition
}

- init;
+ (void) registerFromDictionary:(NSDictionary *)dictionary;
- (void) finalize;	// needed because we're a plugin class
- (void) update;
- (void) left;
- (void) right;
- (void) forward;
- (void) slow;
- (void) autopilot;
- (void) orbit;
- (void) toggleOrbit;
- (void) hyper;
- (void) land;
- (void) fire:(BOOL)primary;
- (id) newInstance;
- (void) addMod:(id)mod;
- hasMod:(id)mod;	// returns mod if contained, nil if not
- (void) hitBy:(Weapon *)weapon;	// just got hit
- (BOOL) addEnemy:(Spaceship *)enemy;	// returns YES if enemy added, NO if already there
- (void) bracket;
- (void) up;
- (void) down;
- (void) throttle:(BOOL)forward;
- (void) afterburner;

@end
