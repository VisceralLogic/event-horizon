//
//  System.h
//  Event Horizon
//
//  Created by user on Sat Sep 21 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "Planet.h"
#import "Government.h"
#import "Type.h"

@interface Solarsystem : NSObject {
@public
	float x, z;
	NSString *name;
	NSMutableArray *planets;
	NSMutableArray *links;
	
	NSString *ID;					// used for player data
	NSMutableArray *initData;		// used to store data between init and finalize
	NSMutableDictionary *flags;		// generic flags
	int shipCount;					// average num of ships in system
	NSString *description;			// description of this sytem for map
	int asteroids;					// number of asteroids in system
	Government *gov;				// which government this system belongs to
	Type *types;					// which ships should appear in this system
	int backdrops;					// number of backdrops, max of 4
	int backdropAngle[4];			// angle for each backdrop
	int backdropElev[4];			// elevation in pixels
	NSString *backdropPath[4];		// path to backdrop graphic
	bool displayOnMap;				// show system info on map
}

+ (void) registerFromDictionary:(NSDictionary *)dictionary;
- (void) finalize;
- (void) setUp;		// the player just entered
- (NSDictionary *) flags;
- (void) update;

- (NSMutableArray *) planets;
- (NSMutableArray *) links;

@end
