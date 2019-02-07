//
//  WimpAI.m
//  Event Horizon
//
//  Created by user on Tue Jun 10 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "WimpAI.h"
#import "Controller.h"

@implementation WimpAI

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
	if( bound ){
	} else {
		if( curPlanet ){
			goal = DO_LAND;
			target = curPlanet;
		} else
			goal = DO_LEAVE;
		if( [enemies count] > 0 ){
			float d = 150;
			if( [sys->planets count] > 0 ){
				int i;
				for( i = 0; i < [sys->planets count]; i++ ){
					Planet *p = [sys->planets objectAtIndex:i];
					if( p->x*p->x + p->z*p->z + p->y*p->y < d*d ){
						d = sqrt(p->x*p->x + p->z*p->z + p->y*p->y);
						curPlanet = p;
						target = p;
						goal = DO_LAND;
					}
				}
			}
			if( d > 15 ){
				target = nil;
				goal = DO_LEAVE;
			}
		}
	}
	[super update];
}

@end
