//
//  Asteroid.m
//  Event Horizon
//
//  Created by user on Wed May 07 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Asteroid.h"
#import "Controller.h"

GLuint asteroidList;

@implementation Asteroid

+ (void) initialize{
	[super initialize];
	
	asteroidList = glGenLists( 1 );
	glNewList( asteroidList, GL_COMPILE );
	gluSphere( sphereObj, 1, 10, 10 );
	glEndList();
}

- init{
	self = [super init];
	[self reset];
	return self;
}

- (void) reset{
	size = .1-.1*sin((pi*random())/RAND_MAX);
	x = (100.0f*random())/RAND_MAX - 50;
	z = (100.0f*random())/RAND_MAX - 50;
	y = (30.0f*random())/RAND_MAX - 15;
	mass = size*5000;
	speedx = ((float)random())/RAND_MAX - .5;
	speedz = ((float)random())/RAND_MAX - .5;
	speedy = ((float)random())/RAND_MAX - .5;
	deltaRot = -400*size+10;
}

- (void) update{
	x += speedx*sys->FACTOR;
	z += speedz*sys->FACTOR;
	y += speedy*sys->FACTOR;
	MAX_VELOCITY = sqrt(speedx*speedx + speedz*speedz + speedy*speedy);
	if(  MAX_VELOCITY > 4 ){
		speedx *= 4/MAX_VELOCITY;
		speedz *= 4/MAX_VELOCITY;
		speedy *= 4/MAX_VELOCITY;
	}
	angle += deltaRot*sys->FACTOR;
	[self collide:sys->planets];
}

- (void) draw{
	if( ![self visible] )
		return;
	[self position];
	glBindTexture( GL_TEXTURE_2D, sys->texture[ASTEROID_TEXTURE] );
	glRotatef( -90, 1, 0, 0 );
	glScalef( size, size, size );
	glCallList( asteroidList );
	//	gluSphere( sphereObj, size, 10, 10 );
	if( sqr(x-sys->x)+sqr(z-sys->z)+sqr(y-sys->y) > 2500 ){	// out of range
		[self reset];
	}	
}

@end
