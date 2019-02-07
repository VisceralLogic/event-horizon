//
//  MyWindow.m
//  Event Horizon
//
//  Created by Paul Dorman on Sun Jun 27 2004.
//  Copyright (c) 2004 Paul Dorman. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "EHButton.h"
#import "MyWindow.h"

extern void (* nsEventScene)( NSEvent *event );
extern BOOL mouseMoved;

@implementation MyWindow

- (void) keyDown:(NSEvent *)event{
	
}

- (void) sendEvent:(NSEvent *)event{
	extern long keyMap5;

	if( nsEventScene )
		nsEventScene( event );
		
	if( [event type] == NSLeftMouseDown && !(keyMap5 & 0x1) )
		keyMap5 |= 0x1;
	else if( [event type] == NSLeftMouseUp && (keyMap5 & 0x1) )
		keyMap5 &= 0xFFFFFFFE;
	else if( [event type] == NSRightMouseDown && !(keyMap5 & 0x2) )
		keyMap5 |= 0x2;
	else if( [event type] == NSRightMouseUp && (keyMap5 & 0x2) )
		keyMap5 &= 0xFFFFFFFD;
	else if( [event type] == NSScrollWheel ){
		if( [event deltaY] > 0 )
			keyMap5 += 0x4;
		else
			keyMap5 += 0x8;
	} else if( [event type] == NSMouseMoved || [event type] == NSLeftMouseDragged || [event type] == NSRightMouseDragged )
		mouseMoved = YES;

	[EHButton handleNSEvent:event];
	[super sendEvent:event];
}

@end