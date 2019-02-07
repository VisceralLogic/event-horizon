//
//  BackgroundMusic.h
//  Event Horizon
//
//  Created by Paul Dorman on 11/12/06.
//  Copyright 2006 Paul Dorman. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface BackgroundMusic : NSObject {

}

+ (void) startMusic;
- (void) musicThread;

@end
