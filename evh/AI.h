//
//  AI.h
//  Event Horizon
//
//  Created by user on Tue Jan 07 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Spaceship.h"

@interface AI : Spaceship {
@public
	SpaceObject* target;		// what the current goal applies to
	float startExplode;		// time ship was killed
	enum {
		DO_LAND,		// land on target planet
		DO_ATTACK,		// attack target spaceship
		DO_LEAVE,		// jump to target system
		DO_LOITER,		// orbit target planet
		DO_ESCORT		// escort target ship
	} goal;
}

- initWithShip:(Spaceship *)ship;
- (void) update;		// control this ship for one step
- (void) land;			// overrides Spacship function
- (void) draw;			// draw us
- (void) drawExplosionSize:(float)size atX:(float)x z:(float)z;
- (void) navigate:(NSArray *)objects;		// steer around these
- (void) doLand;		// perform landing goal
- (void) doAttack;
- (void) doLeave;
- (void) doLoiter;
- (void) doEscort;

@end
