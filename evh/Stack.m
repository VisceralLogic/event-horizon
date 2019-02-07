//
//  Stack.m
//  Event Horizon
//
//  Created by user on Sat Jul 12 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "ControllerGraphics.h"
#import "ControllerThreading.h"
#import "Stack.h"
#import "main.h"
#import "Map.h"
#import "Inventory.h"
#import "Prefs.h"
#import "Planet.h"
#import "Board.h"
#import "Controller.h"
#import "Interface.h"

NSMutableArray *wordStack;

extern void DrawGLScene();

@implementation Stack

+ (void) initialize{
	wordStack = [[NSMutableArray alloc] initWithCapacity:2];
	[wordStack addObject:@"Main"];
}

+ (void) pushWithScene:(NSString *)scene{
	if( [wordStack count] == 1 ){
		beginOrtho();
		ShowCursor();
		CGAssociateMouseAndMouseCursorPosition( YES );
		[pauseLock lock];
	}
	[wordStack removeObject:scene];
	[wordStack addObject:scene];
	drawScene = [Stack drawFor:scene];
	eventScene = [Stack eventFor:scene];
	nsEventScene = [Stack nsEventFor:scene];
	refreshTime = 0.01f;
}

+ (void) pop{
	NSString *scene;
	[wordStack removeLastObject];
	scene = [wordStack lastObject];
	drawScene = [Stack drawFor:scene];
	eventScene = [Stack eventFor:scene];
	nsEventScene = [Stack nsEventFor:scene];
	if( [wordStack count] == 1 ){
		endOrtho();
		HideCursor();
		if( sys->floatVal[MOUSE] == 1.0f )
			CGAssociateMouseAndMouseCursorPosition( NO );
		[pauseLock unlock];
	}
}

+ (void *) drawFor:(NSString *)scene{
	if( [scene isEqualToString:@"Map"] )
		return drawMap;
	if( [scene isEqualToString:@"Planet"] )
		return drawPlanet;
	if( [scene isEqualToString:@"Prefs"] )
		return drawPrefs;
	if( [scene isEqualToString:@"Inventory"] )
		return drawInventory;
	if( [scene isEqualToString:@"Board"] )
		return drawBoard;
	return DrawGLScene;
}

+ (void *) eventFor:(NSString *)scene{
	if( [scene isEqualToString:@"Map"] )
		return eventMap;
	if( [scene isEqualToString:@"Planet" ] )
		return eventPlanet;
	if( [scene isEqualToString:@"Prefs" ] )
		return eventPrefs;
	if( [scene isEqualToString:@"Inventory" ] )
		return eventInventory;
	if( [scene isEqualToString:@"Board"] )
		return eventBoard;
	return NULL;
}

+ (void *) nsEventFor:(NSString *)scene{
	if( [scene isEqualToString:@"Map"] )
		return nsEventMap;
	if( [scene isEqualToString:@"Planet"] )
		return nsEventPlanet;
	if( [scene isEqualToString:@"Prefs"] )
		return nsEventPrefs;
	if( [scene isEqualToString:@"Inventory"] )
		return nsEventInventory;
	if( [scene isEqualToString:@"Board"] )
		return nsEventBoard;
	return NULL;
}

@end