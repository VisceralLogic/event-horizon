//
//  StarDust.h
//  Event Horizon
//
//  Created by Paul Dorman on Sat Jul 30 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGL/gl.h>
#import "SpaceObject.h"

@interface StarDust : SpaceObject {
}

+ (void) initialize;
+ (void) update;

- init;
- (void) update;

@end
