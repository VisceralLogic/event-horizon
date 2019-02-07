//
//  EHButton.h
//  Event Horizon
//
//  Created by Paul Dorman on Fri Jun 17 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Carbon/Carbon.h>

@interface EHButton : NSObject{
@public
	int x, y;
	NSString *text;
	void (* call)( void );
	void (* callParam)( NSString *param );
	int active;
	int width;
	int (* isActive)( void );

@protected
	BOOL pressed;
	BOOL clicked;
	BOOL mouseIn;
}

+ (void) initialize;
+ (void) handleEvent:(EventRef)event;
+ (void) handleNSEvent:(NSEvent *)event;

+ newButton:(NSString *)name atX:(int)x y:(int)y width:(int)width callback:(void *)callback withParam:(BOOL)param active:(BOOL)active isActive:(int *)isActive;
+ (void) update;
+ (void) clear;
+ (void) removeByName:(NSString *)name;
+ (void) remove:(EHButton *)button;
+ (EHButton *) buttonWithName:(NSString *)name;
+ (void) setName:(NSString *)newName forName:(NSString *)oldName;
+ (void) storeSet:(NSString *)name;
+ (void) recallSet:(NSString *)name;
+ (void) clearSet:(NSString *)name;

- (void) draw;
- (BOOL) clicked;
- (BOOL) inBoundsX:(int)x y:(int)y;

@end