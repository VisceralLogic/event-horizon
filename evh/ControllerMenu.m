//
//  ControllerMenu.m
//  Event Horizon
//
//  Created by Paul Dorman on 3/6/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "ControllerMenu.h"
#import "EHMenu.h"
#import "ControllerThreading.h"

@implementation Controller (ControllerMenu)

- (void) initMenus{
	EHMenuItem *resumeItem = [[EHMenuItem alloc] initWithName:@"Resume" hotkey:keyValue( index[PAUSE_KEY], val[PAUSE_KEY] ) menu:nil width:128 target:self action:@selector(pause)];
	[EHMenu newMenuWithTitle:@"Pause" items:[NSArray arrayWithObject:resumeItem]];
	
	EHMenuItem *quitYes = [[EHMenuItem alloc] initWithName:@"Yes" hotkey:"Y" menu:nil width:128 target:self action:@selector(quit:) object:self];
	EHMenuItem *quitNo = [[EHMenuItem alloc] initWithName:@"No" hotkey:"N" menu:nil width:128 target:self action:@selector(quit:) object:nil];
	[EHMenu newMenuWithTitle:@"Really Quit?" items:[NSArray arrayWithObjects:quitYes, quitNo, nil]];
	
	EHMenuItem *quitItem = [[EHMenuItem alloc] initWithName:@"Quit" hotkey:"Q" menu:@"Really Quit?" width:128 target:nil action:nil];
	EHMenuItem *pauseItem = [[EHMenuItem alloc] initWithName:@"Pause" hotkey:keyValue( index[PAUSE_KEY], val[PAUSE_KEY] ) menu:nil width:128 target:self action:@selector(pause)];
	EHMenuItem *hyperItem = [[EHMenuItem alloc] initWithName:@"Hyperspace" hotkey:keyValue( index[HYPER], val[HYPER] ) menu:@"Hyperspace" width:164 target:nil action:nil];
	EHMenuItem *fullItem = [[EHMenuItem alloc] initWithName:@"Fullscreen" hotkey:keyValue( index[FULL], val[FULL] ) menu:nil width:128 target:self action:@selector(toggleFullscreen)];
	EHMenuItem *prefItem = [[EHMenuItem alloc] initWithName:@"Preferences" hotkey:keyValue( index[PREF], val[PREF] ) menu:nil width:128 target:self action:@selector(preferences)];
	[EHMenu newMenuWithTitle:@"Main Menu" items:[NSArray arrayWithObjects:hyperItem, fullItem, prefItem, pauseItem, quitItem, nil]];
}

- (void) toggleFullscreen{
	toggleFull = YES;
}

- (void) pause{
	pause = !pause;
	if( !full ){
		if( pause ){
			ShowCursor();
			CGAssociateMouseAndMouseCursorPosition( YES );
		} else {
			HideCursor();
			if( floatVal[MOUSE] == 1.0f )
				CGAssociateMouseAndMouseCursorPosition( NO );
		}
	}
	if( pause ){
		[EHMenu displayMenu:@"Pause"];	
		[pauseLock lock];
	} else{
		[pauseLock unlock];
	}
}

- (void) preferences{
	setUpPrefs();
}

- (void) map{
	setUpMap();
}

- (void) selectSystem:(NSString *)_name{
	int i;
	for( i = 0; i < [system->links count]; i++ ){
		if( [((Solarsystem*)[system->links objectAtIndex:i])->name isEqualToString:_name] ){
			systemIndex = i;
			break;
		}
	}
}

- (void) quit:(id)sure{
	if( sure ){
		QuitApplicationEventLoop();
		if( !full )
			toggleFull = YES;		
	}
}

@end
