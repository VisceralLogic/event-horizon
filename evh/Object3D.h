//
//  Object3D.h
//  Event Horizon
//
//  Created by user on Sat Mar 01 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "SpaceObject.h";
#import "Coord.h";
#import "OBJ.h"

@interface Object3D : SpaceObject {
@public
	NSMutableArray *tri;	// arrays of coords defining tri, quad, poly
	NSMutableArray *quad;
	NSMutableArray *poly;	// array of array of coord
	GLuint listNum;			// number of lists
	GLuint *list;			// gl drawing lists
	BOOL obj;				// is this a WaveFront OBJ?
	NSMutableArray *objects;
	int price;				// how much it costs to buy
}

- init;
- (void) load:(NSData *)data withTexture:(NSString *)tex;			// load 3D data from file
- (void) drawObject;	// draw self
- (void) listDraw;		// make gl calls for drawing list
- (void) objListDraw:(OBJ *)o;	// gl calls for OBJ file
- (void) loadOBJ:(NSString *)file;

@end
