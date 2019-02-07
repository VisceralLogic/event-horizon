//
//  Console.h
//  Event Horizon
//
//  Created by Paul Dorman on Sat Feb 26 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>

@interface Console : NSObject {

}

+ (void) draw;
+ (void) appendChar:(char)c;
+ (void) setup;
+ (void) end;
+ (void) print:(NSString *)str;
+ (void) dump:(id) data;

@end

void eventConsole( EventRef event );
void nsEventConsole( NSEvent *event );