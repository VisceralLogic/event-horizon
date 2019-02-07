//
//  Asteroid.h
//  Event Horizon
//
//  Created by user on Wed May 07 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "SpaceObject.h"
#import "Object3D.h"

@interface Asteroid : Object3D {

}

- init;
- (void) draw;
- (void) update;
- (void) reset;

@end
