//
//  ControllerThreading.h
//  Event Horizon
//
//  Created by Paul Dorman on 3/19/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "Controller.h"

extern float threadTime;
extern NSLock *pauseLock, *asteroidLock, *shipLock, *weaponLock;

@interface Controller (ControllerThreading)

- (void) startThreads;
- (void) planetThread;
- (void) updatePlanets;
- (void) asteroidThread;
- (void) updateAsteroids;
- (void) shipThread;
- (void) updateShips;
- (void) weaponThread;
- (void) updateWeapons;

@end