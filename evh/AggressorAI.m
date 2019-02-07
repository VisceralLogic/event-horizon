//
//  AggressorAI.m
//  Event Horizon
//
//  Created by user on Fri Jul 04 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "AggressorAI.h"
#import "Controller.h"

@implementation AggressorAI

- init{
	self = [super init];
	return self;
}

- (void) update{
	int i;

	for( i = 0; i < [enemies count]; i++ ){
		Spaceship *ship = [enemies objectAtIndex:i];
		if( ship->state == DEAD ){
			[enemies removeObject:ship];
			i--;
		}
	}
	if( [enemies count] == 0 ){			// if no enemies, look for new
		if( escortee ){
			for( i = 0; i < [escortee->enemies count]; i++ )
				[self addEnemy:[escortee->enemies objectAtIndex:i]];
		}
		else {
			for( i = 0; i < [sys->ships count]; i++ ){
				Spaceship *ship = [sys->ships objectAtIndex:i];
				if( ship == self )
					continue;
				if( ship == sys && [[sys->govRecord objectForKey:gov->ID] intValue] < 0 )
					[self addEnemy:sys];
				else if( ship != sys ){
					if( ship->gov ){
						if( [gov->enemies containsObject:ship->gov->ID] )
							[self addEnemy:ship];
					} else {	// see if it doesn't have gov because it's escort
					
						
					}
				}
			}
		}
	}
	
	if( bound ){
	} else if( [enemies count] > 0 && goal != DO_ATTACK ){
		target = selection = [enemies objectAtIndex:((float)random())/RAND_MAX*[enemies count]];
		goal = DO_ATTACK;
	} else {
		goal = DO_LEAVE;
		if( escortee )
			goal = DO_ESCORT;
	}
	[super update];
}

@end
