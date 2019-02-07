//
//  Coord.h
//  Ship Builder
//
//  Created by user on Tue Dec 31 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <math.h>
#import <OpenGL/gl.h>

@interface Coord : NSObject {
@public
	float x, y, z;		// model coords
	float u, v;			// texture coords
}

- (void) makeOBJTex;
- (void) makeTexCoord;
- (void) makeVertex;
- (void) makeNormal;
- (float) xzDistance:(Coord*)c;
- (float) xyDistance:(Coord*)c;
- (float) yzDistance:(Coord*)c;
- (float) xyzDistance:(Coord*)c;
- (float) uvDistance:(Coord*)c;
- (BOOL) equals:(Coord*)c;


@end
