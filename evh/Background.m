//
//  Background.m
//  Event Horizon
//
//  Created by user on Mon Mar 17 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Background.h"
#import "Controller.h"

GLuint starList;

@implementation Background

+ (void) initialize{
	[super initialize];

	starList = glGenLists( 1 );
	glNewList( starList, GL_COMPILE );
	gluSphere( sphereObj, .5, 3, 3 );
	glEndList();
}

- init{
	self = [super init];
	x = malloc(1);
	y = malloc(1);
	z = malloc(1);
	count = 1;
	return self;
}

- (void) dealloc{
	free( x );
	free( y );
	free( z );
	
	[super dealloc];
}

- (void) generate:(float)density{
	int i;

	free( x );
	free( y );
	free( z );
	count = 1000*density;
	x = malloc( sizeof(int)*count );
	y = malloc( sizeof(int)*count );
	z = malloc( sizeof(int)*count );
	
	for( i = 0; i < count; i++ ){
		float temp = 2.0f*(pi*random())/RAND_MAX;
		x[i] = 450*sin(temp);
		y[i] = 450*cos(temp);
		temp = (pi*random())/RAND_MAX-pi/2;
		z[i] = 450*sin(temp);
		x[i] *= cos(temp);
		y[i] *= cos(temp);
	}
}

- (void) draw{
	int i;
	
	glColor3f( 1, 1, 1 );
	for( i = 0; i < count; i++ ){
		glLoadIdentity();
		if( sys->viewStyle == 0 ){
			glRotatef( -sys->pitch, 1, 0, 0 );
			glRotatef( -sys->roll, 0, 0, 1 );
			glRotatef( -sys->angle, 0, 1, 0 );
		}  else if( sys->viewStyle == 2 ){
			glRotatef( sys->pitch, 1, 0, 0 );
			glRotatef( sys->roll, 0, 0, 1 );
			glRotatef( 180-sys->angle, 0, 1, 0 );
		} else if( sys->viewStyle == 3 ){
			glRotatef( sys->deltaRot/10, 0, 1, 0 );
			glRotatef( sys->deltaPitch/10, 1, 0, 0 );
			glRotatef( -sys->pitch, 1, 0, 0 );
			glRotatef( -sys->roll, 0, 0, 1 );
			glRotatef( -sys->angle, 0, 1, 0 );
		}
		glTranslatef( x[i], y[i], z[i] );
		glCallList( starList );
	}
}

@end
