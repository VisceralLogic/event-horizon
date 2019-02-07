//
//  Forklift.m
//  Event Horizon
//
//  Created by Paul Dorman on 6/19/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Forklift.h"

Forklift *forklift;
Mod *launcher;
BOOL isForkliftInstalled = NO;

@implementation Forklift

+ (void) install{
	if( !isForkliftInstalled ){
		[sys addMod:forklift];
		[sys addMod:launcher];
		isForkliftInstalled = YES;		
	}
}

+ (void) setup{
	// the Forklift
	forklift = [[Forklift alloc] init];
	[forklift loadOBJ:[[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/Forklift.obj"]];
	forklift->name = @"Forklift";
	forklift->ID = @"special.weapons.Forklift";
	forklift->ANGULAR_ACCELERATION = 5;
	forklift->MAX_ANGULAR_VELOCITY = 36;
	forklift->ACCELERATION = 20;
	forklift->MAX_VELOCITY = 1.5;
	forklift->mass = 0;
	forklift->size = .08;
	forklift->life = 90;
	forklift->isPrimary = NO;
	forklift->description = @"The Forklift... nothing more need be said.";
	forklift->price = 0;
	forklift->reload = 8;
	forklift->force = 100;
	forklift->max = -1;
	forklift->inAcc = 0;
	forklift->launchStyle = STRAIGHT;
	forklift->proximity = 0.5;
	forklift->disableLighting = NO;
	forklift->count = 1;

	// the launcher
	launcher = [[Mod alloc] init];
	launcher->name = @"Forklift Launcher";
	launcher->ID = @"special.mods.ForkliftLauncher";
	launcher->mass = 0;
	launcher->description = @"Launches the Almighty Forklift";
	launcher->price = 0;
	launcher->max = 1;
	launcher->modType = LAUNCHER;
	
	forklift->launcher = launcher;
	launcher->ammo = forklift;
}

- (void) newInstanceFrom:(Spaceship *)_source target:(Spaceship *)_target{
	[super newInstanceFrom:_source target:_target];
	count = 2;
}

- shipInstance{
	return forklift;
}

@end
