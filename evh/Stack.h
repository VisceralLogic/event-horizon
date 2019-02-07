//
//  Stack.h
//  Event Horizon
//
//  Created by user on Sat Jul 12 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Carbon/Carbon.h>

@interface Stack : NSObject {
}

+ (void) initialize;
+ (void) pushWithScene:(NSString *)scene;		// add this scene to the stack
+ (void) pop;									// remove current scene, go to last
+ (void *) drawFor:(NSString *)scene;			// get drawScene for scene
+ (void *) eventFor:(NSString *)scene;			// get eventScene for scene
+ (void *) nsEventFor:(NSString *)scene;		// get nsSeventScene

@end