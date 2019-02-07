//
//  ControllerThreading.m
//  Event Horizon
//
//  Created by Paul Dorman on 3/19/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "ControllerThreading.h"

float threadTime = 0.01f;
NSLock *pauseLock, *shipLock, *asteroidLock, *weaponLock;

@implementation Controller (ControllerThreading)

- (void) initialize{
	pauseLock = [[NSLock alloc] init];
}

- (void) startThreads{
	[NSThread detachNewThreadSelector:@selector(planetThread) toTarget:self withObject:nil];
	[NSThread detachNewThreadSelector:@selector(asteroidThread) toTarget:self withObject:nil];
	[NSThread detachNewThreadSelector:@selector(shipThread) toTarget:self withObject:nil];
	[NSThread detachNewThreadSelector:@selector(weaponThread) toTarget:self withObject:nil];
}

- (void) planetThread{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSTimer *timer = [NSTimer timerWithTimeInterval:threadTime target:self selector:@selector(updatePlanets) userInfo:nil repeats:NO];
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    [[NSRunLoop currentRunLoop] run];
	[pool release];
}

- (void) updatePlanets{
	NSTimer *timer;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int i;
	
	if( !pause ){
		for( i = 0; i < [planets count]; i++ ){
			[[planets objectAtIndex:i] update];
		}		
	}
	
	timer = [NSTimer scheduledTimerWithTimeInterval:threadTime target:self selector:@selector(updatePlanets) userInfo:nil repeats:NO];
    [pool release];	
}

- (void) asteroidThread{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSTimer *timer = [NSTimer timerWithTimeInterval:threadTime target:self selector:@selector(updateAsteroids) userInfo:nil repeats:NO];
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    [[NSRunLoop currentRunLoop] run];
	[pool release];
}


- (void) updateAsteroids{
//	[pauseLock lock];
//	[pauseLock unlock];
	NSTimer *timer;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int i;
	
	if( !pause ){
		[asteroidLock lock];
		for( i = 0; i < [asteroids count]; i++ ){
			[[asteroids objectAtIndex:i] update];
		}
		[asteroidLock unlock];		
	}
	
	timer = [NSTimer scheduledTimerWithTimeInterval:threadTime target:self selector:@selector(updateAsteroids) userInfo:nil repeats:NO];
    [pool release];	
}

- (void) shipThread{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSTimer *timer = [NSTimer timerWithTimeInterval:threadTime target:self selector:@selector(updateShips) userInfo:nil repeats:NO];
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    [[NSRunLoop currentRunLoop] run];
	[pool release];
}


- (void) updateShips{
	NSTimer *timer;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int i;
	
	if( !pause ){
		[shipLock lock];
		for( i = 0; i < [ships count]; i++ ){
			AI *ship = [ships objectAtIndex:i];
			if( ship->state == DISABLED || ship->state == DEAD )
				[enemies removeObject:ship];
			if( ship->state == DEAD )
				[ships removeObjectAtIndex:i--];
			if( ship != sys )
				[ship update];
		}
		[shipLock unlock];
	}
	
	timer = [NSTimer scheduledTimerWithTimeInterval:threadTime target:self selector:@selector(updateShips) userInfo:nil repeats:NO];
    [pool release];	
}

- (void) weaponThread{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSTimer *timer = [NSTimer timerWithTimeInterval:threadTime target:self selector:@selector(updateWeapons) userInfo:nil repeats:NO];
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    [[NSRunLoop currentRunLoop] run];
	[pool release];
}


- (void) updateWeapons{
//	[pauseLock lock];
//	[pauseLock unlock];
	NSTimer *timer;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int i;
	
	if( !pause ){
		[weaponLock lock];
		for( i = 0; i < [weaps count]; i++ ){
			[[weaps objectAtIndex:i] update];
		}
		[weaponLock unlock];		
	}
	
	timer = [NSTimer scheduledTimerWithTimeInterval:threadTime target:self selector:@selector(updateWeapons) userInfo:nil repeats:NO];
    [pool release];	
}

@end
