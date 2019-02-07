//
//  Mod.h
//  Event Horizon
//
//  Created by user on Tue Apr 08 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "SpaceObject.h"
#import "ModProtocol.h"

@interface Mod : NSObject <ModProtocol> {
@public
	int mass;			// how much room this takes
	int price;			// how much it costs
	GLuint modPic;		// texture for buying
	NSString *description;	// description for buying
	NSMutableDictionary *flags;	// various flags
	NSString *name;		// name displayed
	NSString *ID;		// universal ID
	int count;			// how many of this mod on ship
	int max;			// max number on a ship
	id ammo;			// weapon pointer if this is a launcher
	id initData;		//
	NSString *flagRequirements;	// needed on planet to be sold there
	enum{				// type of mod this is
		LAUNCHER,			// secondary weapon launcher
		COMPENSATOR,			// inertial compensator
		FUELTANK,			// additional fuel storage
		AFTERBURNER,		// activated mod, adds thrust
	} modType;
	float value;		// various, depending on type of mod
	float value2;
	float value3;
}

+ (void) registerFromDictionary:(NSDictionary *)dict;
- (void) finalize;
- (int) price;
- (int) mass;
- (NSString *) ID;
- (void) incrementCount;
- (int) count;
- (int) max;
- (NSMutableDictionary *) flags;
- (NSString *) desc;
- (GLuint) texture;
- (NSString *) name;
- shipInstance;
- (NSString *) flagRequirements;

@end
