//
//  EHMenuItem.h
//  Event Horizon
//
//  Created by Paul Dorman on 2/16/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "EHMenu.h"
#import "KeyMapNames.h"

@interface EHMenuItem : NSObject {
@public
	NSString *name;			// name to display
	keyCode hotkey;			// key to respond to
	enum{
		ACTIVE,
		SELECTED
	} state;				// current display state
	NSString *menu;			// menu for submenu items
	int width;				// menu width
	id target;				// object to perform action on
	SEL action;				// action to perform
	id object;				// objec to pass in action
	BOOL objectPresent;		// if there is an object to pass
}

- initWithName:(NSString *)name hotkey:(const char *)keyName menu:(NSString *)menu width:(int)width target:(id)target action:(SEL)action;
- initWithName:(NSString *)name hotkey:(const char *)keyName menu:(NSString *)menu width:(int)width target:(id)target action:(SEL)action object:(id)object;
- (int) drawAtX:(int)x y:(int)y;
- (void) perform;

@end
