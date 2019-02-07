//
//  Interface.h
//  Event Horizon
//
//  Created by Paul Dorman on Sun Jun 27 2004.
//  Copyright (c) 2004 Paul Dorman. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "main.h"

extern float refreshTime;

@interface Interface : NSObject {
	NSTimer *timer;
}

- (void) timer;
- (IBAction) terminate:(id)sender;
- (IBAction) hideOtherApplications:(id)sender;
- (IBAction) hide:(id)sender;
- (IBAction) unhideAllApplications:(id)sender;
- (IBAction) orderFrontStandardAboutPanel:(id)sender;
- (IBAction) fullScreen:(id)sender;
- (IBAction) changeResolution:(id)sender;
- (void) mouseDown:(NSEvent *)event;

@end
