//
//  ShieldSpot.m
//  Event Horizon
//
//  Created by Paul Dorman on Wed Aug 17 2005.
//  Copyright (c) 2005 Paul Dorman. All rights reserved.
//

#import "ShieldSpot.h"
#import "Controller.h"

GLuint shieldSpotTexture;

@implementation ShieldSpot

- initAtX:(float)x y:(float)y z:(float)z time:(float)time size:(float)_size{
	float mag;
	
	self = [super init];

	self->x = x;
	self->y = y;
	self->z = z;
	start = time;
	size = _size;
	
	// find a normal unit vector
	a = -y;
	b = x;
	c = 0;
	mag = sqrt(a*a+b*b);
	a /= mag;
	b /= mag;
	
	u = -z*b;
	v = z*a;
	w = x*b-y*a;
	mag = sqrt(u*u+v*v+w*w);
	u /= mag;
	v /= mag;
	w /= mag;
	
	return self;
}

- (void) update:(float)percent{
	float scale = size*(sys->t-start);
	if( sys->t - start - 1.0f > 0 ){
		done = YES;
		return;
	}
	glColor4f( 0.5-0.5*percent, 0.5+0.5*percent, 0.5+0.5*percent, 1+start-sys->t );
	glBindTexture( GL_TEXTURE_2D, shieldSpotTexture );
	glBegin( GL_QUADS );
		glTexCoord2f( 0, 0 );
		glVertex3f( x+a*scale, y+b*scale, z+c*scale );
		glTexCoord2f( 0, 1 );
		glVertex3f( x+u*scale, y+v*scale, z+w*scale );
		glTexCoord2f( 1, 1 );
		glVertex3f( x-a*scale, y-b*scale, z-c*scale );
		glTexCoord2f( 1, 0 );
		glVertex3f( x-u*scale, y-v*scale, z-w*scale );
	glEnd();
}

@end
