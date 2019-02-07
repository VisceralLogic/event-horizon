//
//  OBJ.m
//  NativeShipBuilder
//
//  Created by Paul Dorman on Mon Apr 26 2004.
//  Copyright (c) 2004 Paul Dorman. All rights reserved.
//

#import "OBJ.h"


@implementation OBJ

- init{
	self = [super init];
	vertices = [[NSMutableArray alloc] initWithCapacity:20];
	normals = [[NSMutableArray alloc] initWithCapacity:20];
	uvCoords = [[NSMutableArray alloc] initWithCapacity:20];
	faces = [[NSMutableArray alloc] initWithCapacity:20];
	return self;
}

@end
