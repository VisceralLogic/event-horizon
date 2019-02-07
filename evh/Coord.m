//
//  Coord.m
//  Ship Builder
//
//  Created by user on Tue Dec 31 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "Coord.h"


@implementation Coord

- (void) makeOBJTex{
	glTexCoord2f( x, y );
}

- (void) makeTexCoord{
	glTexCoord2f( u, 1-v );
}

- (void) makeVertex{
	glVertex3f( x, y, z );
}

- (void) makeNormal{
	glNormal3f( x, y, z );
}

- (float) xzDistance:(Coord*)c{
	return sqrt( sqr(x-c->x) + sqr(z-c->z) );
}

- (float) xyDistance:(Coord*)c{
	return sqrt( sqr(x-c->x) + sqr(y-c->y) );
}

- (float) yzDistance:(Coord*)c{
	return sqrt( sqr(y-c->y) + sqr(z-c->z) );
}

- (float) xyzDistance:(Coord*)c{
	return sqrt( sqr(x-c->x) + sqr(y-c->y) + sqr(z-c->z) );
}

- (float) uvDistance:(Coord*)c{
	return sqrt( sqr(u-c->u) + sqr(v-c->v) );
}

- (BOOL) equals:(Coord*)c{
	return (x==c->x && y==c->y && z==c->z);
}

- (void) describe{
	NSLog( @"<%f, %f, %f>", x, y, z );
}

@end