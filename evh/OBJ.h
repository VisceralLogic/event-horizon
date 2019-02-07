//
//  OBJ.h
//  NativeShipBuilder
//
//  Created by Paul Dorman on Mon Apr 26 2004.
//  Copyright (c) 2004 Paul Dorman. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Coord.h"

@interface OBJ : NSObject {
@public
	NSMutableArray *vertices;
	NSMutableArray *normals;
	NSMutableArray *uvCoords;
	NSMutableArray *faces;
	NSString *name;
	NSString *mtllib;
	NSString *mtl;
	unsigned long texture;
	BOOL smooth;
}

@end
