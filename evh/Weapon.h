//
//  Weapon.h
//  Event Horizon
//
//  Created by user on Sat Mar 01 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Object3D.h"
#import "Controller.h"
#import "Spaceship.h"
#import "ModProtocol.h"
#import "Mod.h"

@interface Weapon : Object3D <ModProtocol> {
@public
	Spaceship *source;		// firing ship
	Spaceship *target;		// what should be hit
	float life;				// how long it lasts
	float reload;			// time between successive shots
	float lastShot;			// time since last shot
	BOOL isPrimary;			// responds to primary weapon trigger
	int force;				// how much shields/armor depleted
	GLuint modPic;			// texture for mod picture
	int count;				// how many of this weapon does ship have?
	int max;				// max number on a ship
	unsigned int sound;		// sound num for SoundManager
//	NSMovie *sound;			// sound to play when fired
	float proximity;		// how far away before it explodes?
	Mod *launcher;			// launcher required by secondary weapons
	BOOL canFire;			// YES if secondary and launcher present
	float inAcc;			// inaccuracy at start in degrees
	enum {					// how this weapon is launched
		STRAIGHT,
		TURRET,
		SPACESHIP,
		BEAM
	} launchStyle;
	int lastHardpoint;		// where last fired from
	Spaceship *spaceship;	// for bay-launched escort ships
	int proxMass;			// mass for proximity explosion
	BOOL disableLighting;	// full illuminated if true
}

+ (void) registerFromDictionary:(NSDictionary *)dictionary;
- (void) newInstanceFrom:(Spaceship *)source target:(Spaceship *)target;
-		 shipInstance;
- (void) collide:(NSArray *)objects;
- (void) draw;
- (void) update;
- (void) finalize;
- (int) price;
- (int) mass;
- (NSString *) ID;
- (void) incrementCount;
- (int) count;
- (int) max;
- (NSString *) desc;
- (GLuint) texture;
- (NSString *) name;
- (NSMutableDictionary *) flags;
- (NSString *) flagRequirements;

@end
