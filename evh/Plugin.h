//
//  Plugin.h
//  Event Horizon
//
//  Created by user on Mon Dec 09 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Controller.h"

@interface Plugin : NSObject {
	Controller *sys;
}

- initWithController:(Controller *)controller;
- (void) initData;
- (void) loadFile:(NSString *)file forPlugin:(NSString *)name;

@end