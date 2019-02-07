//
//  ShieldSpot.h
//  Event Horizon
//
//  Created by Paul Dorman on Wed Aug 17 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGL/gl.h>

@interface ShieldSpot : NSObject {
@public
	float x, y, z, a, b, c, u, v, w;
	float start;
	float size;
	bool done;
}

- initAtX:(float)x y:(float)y z:(float)z time:(float)time size:(float)size;
- (void) update:(float)percent;

@end
