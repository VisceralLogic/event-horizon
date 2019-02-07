//
//  ControllerMenu.h
//  Event Horizon
//
//  Created by Paul Dorman on 3/6/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import "Controller.h"

@interface Controller (ControllerMenu)

- (void) initMenus;

- (void) toggleFullscreen;
- (void) pause;
- (void) map;
- (void) preferences;
- (void) selectSystem:(NSString *)name;
- (void) quit:(id)sure;		// are you sure you want to quit


@end
