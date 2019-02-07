//
//  MerchantAI.m
//  Event Horizon
//
//  Created by user on Fri Jun 06 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "MerchantAI.h"
#import "Weapon.h"

@implementation MerchantAI

- init{
	self = [super init];

	if( ((float)random())/RAND_MAX > 0.5 ){	// 50% land, 50% leave
		float d = [sys->planets count];
		if( d > 0 )
			curPlanet = [sys->planets objectAtIndex:(int)((d*random())/RAND_MAX)];
	}
	return self;
}

- (void) update{
	int i;
	Spaceship *ship;

	if( bound ){
	} else if( escortee ){
		goal = DO_ESCORT;
		for( i = 0; i < [escortee->enemies count]; i++ ){
			ship = [escortee->enemies objectAtIndex:i];
			if( sqr(escortee->x-ship->x)+sqr(escortee->z-ship->z)+sqr(escortee->y-ship->y) < 25 ){
				target = selection = ship;
				goal = DO_ATTACK;
			}
		}
		for( i = 0; i < [enemies count]; i++ ){
			ship = [enemies objectAtIndex:i];
			if( ship->state == DEAD ){
				[enemies removeObjectAtIndex:i--];
				continue;
			}
			if( sqr(ship->x-x)+sqr(ship->z-z)+sqr(ship->y-y) < 50 && sqr(x-escortee->x)+sqr(z-escortee->z)+sqr(y-escortee->y) < sqr(5*size+escortee->size) ){
				target = selection = ship;
				goal = DO_ATTACK;
			}
		}
	} else {
		if( curPlanet ){
			goal = DO_LAND;
			target = curPlanet;
		} else
			goal = DO_LEAVE;
		for( i = 0; i < [enemies count]; i++ ){
			ship = [enemies objectAtIndex:i];
			if( ship->state == DEAD ){
				[enemies removeObjectAtIndex:i--];
				continue;
			}
			if( sqr(ship->x-x)+sqr(ship->z-z)+sqr(ship->y-y) < 50 ){
				target = selection = ship;
				goal = DO_ATTACK;
			}
		}
	}
	[super update];
}

@end
