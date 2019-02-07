//
//  Mission.h
//  Event Horizon
//
//  Created by user on Wed May 21 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Solarsystem.h"
#import "Planet.h"

@interface Mission : NSObject {
@public
	NSString *availType;	// affects how 'avail' is interpreted ['government'|'system']
	NSString *avail;		//
	NSString *availFlags;	// what flags must be set
	NSString *destType;		// same as 'availType'
	NSString *dest;			//
	Solarsystem *startSystem;	// initialized in newInstanceIn: on:
	Solarsystem *endSystem;		//	...
	Planet *startPlanet;	//	...
	NSString *startFlags;	// which flags to set upon starting
	Planet *endPlanet;		//	...
	NSString *setFlags;		// which flags to set upon completion
	int size;				// how much cargo
	int payment;			// value of mission
	NSString *title;		// mission title
	NSString *description;	// about this mission
	NSString *ID;			// EVH ID
	float freq;				// how often it shows up (0..1)
	NSMutableDictionary *flags;		// whatever
	NSString *endText;		// what you see when you finish it
	NSTimeInterval endTime;	// how long before mission expires
	NSDate *expires;		// date mission expires
	NSString *loadScript;	// F-Script executed on load
	NSString *startScript;	// F-Script executed on accept
	NSString *endScript;	// F-Script executed on complete
}

+ (void) registerFromDictionary:(NSDictionary *)dictionary;
- (void) finalize;
- (Mission*) suitableSystem:(Solarsystem*)system andPlanet:(Planet*)planet;
- (NSDictionary *) flags;
- (Mission*) newInstanceIn:(Solarsystem*)system on:(Planet*)planet;
- (void) accept;
- (void) accomplish;
- (void) update;

@end
