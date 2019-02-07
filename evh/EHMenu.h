//
//  EHMenu.h
//  Event Horizon
//
//  Created by Paul Dorman on 2/16/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import "main.h"

@class EHMenuItem;

@interface EHMenu : NSObject {
@public
	NSMutableArray *items;		// either a menu item (EHMenuItem) or a submenu (EHMenu)
	int x, y;					// location on screen
	EHMenu *parent;				// parent menu for submenus
	int width;					// width set to widest of items
}

- (void) handleChar:(unichar)key;

+ (void) initialize;
+ (void) newMenuWithTitle:(NSString *)title items:(NSArray *)items;
+ (void) displayMenu:(NSString *)title;							// display at default location
+ (void) displayMenu:(NSString *)title atX:(int)x y:(int)y;		// display at given location
+ (void) update:(KeyMap2)keys;	// draw and handle keys
+ (BOOL) active;				// whether there are any menus visible
+ (void) upOne;					// return to parent menu, or game

@end
