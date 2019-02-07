//
//  SpaceObject.m
//  Event Horizon
//
//  Created by user on Wed Aug 07 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "SpaceObject.h"
#import <Cocoa/cocoa.h>
#import "Controller.h"
#import "Texture.h"

#import "stdio.h"

SpaceObject *SPACEOBJECT_ORIGIN;
GLUquadricObj *sphereObj;
Controller *sys;

#define HOLE_FACTOR 1.67
#define SCALE_FACTOR 1.1

@implementation SpaceObject

+ (void) initialize{
	[super initialize];
	SPACEOBJECT_ORIGIN = [[SpaceObject alloc] init];
	SPACEOBJECT_ORIGIN->x = 0;
	SPACEOBJECT_ORIGIN->z = 0;
	SPACEOBJECT_ORIGIN->y = 0;
	sphereObj = gluNewQuadric();
	gluQuadricTexture( sphereObj, GL_TRUE );
	gluQuadricOrientation( sphereObj, GLU_OUTSIDE );
}

- (void) loadTextures:(NSString **)files{
	int i;
	
	for( i = 0; i < texNum; i++ ){
		[Texture loadTexture:files[i] into:&texture[i] withWidth:nil height:nil];
	}
}

- (void) bracket{
	[self position];
	glBindTexture( GL_TEXTURE_2D, 0 );
	size *= SCALE_FACTOR;
	glBegin( GL_LINES );
		glVertex3f( size/HOLE_FACTOR, size, size );
		glVertex3f( -size/HOLE_FACTOR, size, size );
		glVertex3f( size, size/HOLE_FACTOR, size );
		glVertex3f( size, -size/HOLE_FACTOR, size );
		glVertex3f( size, size, size/HOLE_FACTOR );
		glVertex3f( size, size, -size/HOLE_FACTOR );
		
		glVertex3f( -size/HOLE_FACTOR, -size, -size );
		glVertex3f( size/HOLE_FACTOR, -size, -size );
		glVertex3f( -size, -size/HOLE_FACTOR, -size );
		glVertex3f( -size, size/HOLE_FACTOR, -size );
		glVertex3f( -size, -size, -size/HOLE_FACTOR );
		glVertex3f( -size, -size, size/HOLE_FACTOR );
		
		glVertex3f( -size, size/HOLE_FACTOR, size );
		glVertex3f( -size, -size/HOLE_FACTOR, size );
		glVertex3f( -size, size, size/HOLE_FACTOR );
		glVertex3f( -size, size, -size/HOLE_FACTOR );
		
		glVertex3f( size/HOLE_FACTOR, -size, size );
		glVertex3f( -size/HOLE_FACTOR, -size, size );
		glVertex3f( size, -size, size/HOLE_FACTOR );
		glVertex3f( size, -size, -size/HOLE_FACTOR );
		
		glVertex3f( size/HOLE_FACTOR, size, -size );
		glVertex3f( -size/HOLE_FACTOR, size, -size );
		
		glVertex3f( size, size/HOLE_FACTOR, -size );
		glVertex3f( size, -size/HOLE_FACTOR, -size );
	glEnd();
	size /= SCALE_FACTOR;
	glColor3f( 1, 1, 1 );
}

- (void) setOrbit:(SpaceObject *)center{
	centerOfRotation = center;
	if( center != NULL ){
		float m = centerOfRotation->mass;
		theta = atan2( z - centerOfRotation->z, x - centerOfRotation->x )*180/pi;
		distance = sqrt( sqr(x-centerOfRotation->x) + sqr(z-centerOfRotation->z) );
		w = sqrt( sys->GRAVITY*m/(distance*distance) );
	}
}

// Perform collision function with each object in array
- (void) collide:(NSArray *)objects{
	int i;
	
	for( i = 0; i < [objects count]; i++ ){
		SpaceObject *a = [objects objectAtIndex:i];
		if( a != self )
			[self bounce:a];
	}

}

- (void) bounce:(SpaceObject *)sphere{
	float distAdd = 0;
	bool isPlanet = [sphere isKindOfClass:[Planet class]];

	if( self == sys )
		distAdd = isPlanet ? .1 : .05;
	if( !( sqr(sphere->x-x)+sqr(sphere->z-z)+sqr(sphere->y-y) > sqr(size+sphere->size+distAdd) ) ){	// they touch
		float m1 = mass;
		float m2 = isPlanet ? 1E6 : sphere->mass;
		float dvx2, dvy2, dvz2;
		float A, B, dist;
	
		A = (sqr(sphere->x-x)+sqr(sphere->z-z)+sqr(sphere->y-y))*m2*(1.0f+m2/m1);
		if( !isPlanet )
			B = 2*m2*((sphere->x-x)*(sphere->speedx-speedx)+(sphere->y-y)*(sphere->speedy-speedy)+(sphere->z-z)*(sphere->speedz-speedz));
		else
			B = 2*m2*((x-sphere->x)*speedx+(y-sphere->y)*speedy+(z-sphere->z)*speedz);
		
		dvx2 = -B/A*(sphere->x-x);
		dvy2 = -B/A*(sphere->y-y);
		dvz2 = -B/A*(sphere->z-z);
		
		speedx -= m2/m1*dvx2;
		speedy -= m2/m1*dvy2;
		speedy -= m2/m1*dvz2;
		
		dist = sqrt( sqr(sphere->x-x) + sqr(sphere->z-z) + sqr(sphere->y-y) );
		dist = (size+sphere->size+distAdd - dist)/dist;

		if( !isPlanet ){
			sphere->speedx += dvx2;
			sphere->speedy += dvy2;
			sphere->speedz += dvz2;
			
			dist *= m1/(m2+m1);
			sphere->x += (sphere->x-x)*dist;
			sphere->y += (sphere->y-y)*dist;
			sphere->z += (sphere->z-z)*dist;
			dist *= m2/m1;
		}
				
		x += (x-sphere->x)*dist;
		y += (y-sphere->y)*dist;
		z += (z-sphere->z)*dist;
	}
}

- (void) position{
	if( sys->viewStyle == 0 ){
		glLoadIdentity();
		glRotatef( -sys->pitch, 1, 0, 0 );
		glRotatef( -sys->roll, 0, 0, 1 );
		glRotatef( -sys->angle, 0, 1, 0 );			// sys angle
		glTranslatef( x - sys->x, y - sys->y, sys->z - z );	// position self
		glRotatef( angle, 0, 1, 0 );				// rotate to own angle
		glRotatef( roll, 0, 0, 1 );
		glRotatef( pitch, 1, 0, 0 );
	} else if( sys->viewStyle == 1 ){
		glLoadIdentity();
		glRotatef( -sys->angle, 0, 0, 1 );
		glTranslatef( (x-sys->x), (z-sys->z), -15-sys->y+y );
		glRotatef( angle, 0, 0, 1 );
		glRotatef( 90, 1, 0, 0 );
	} else if( sys->viewStyle == 2 ){
		glLoadIdentity();
		glRotatef( sys->pitch, 1, 0, 0 );
		glRotatef( sys->roll, 0, 0, 1 );
		glRotatef( 180-sys->angle, 0, 1, 0 );
		glTranslatef( x-sys->x, y-sys->y, sys->z-z );
		glRotatef( angle, 0, 1, 0 );
		glRotatef( roll, 0, 0, 1 );
		glRotatef( pitch, 1, 0, 0 );
	} else if( sys->viewStyle == 3 ){
		glLoadIdentity();
		glTranslatef( 0, -sys->size, -5*sys->size );
		glRotatef( sys->deltaRot/10, 0, 1, 0 );
		glRotatef( sys->deltaPitch/10, 1, 0, 0 );
		glRotatef( -sys->pitch, 1, 0, 0 );
		glRotatef( -sys->roll, 0, 0, 1 );
		glRotatef( -sys->angle, 0, 1, 0 );
		glTranslatef( x - sys->x, y-sys->y, sys->z - z );
		glRotatef( angle, 0, 1, 0 );
		glRotatef( roll, 0, 0, 1 );
		glRotatef( pitch, 1, 0, 0 );
	}
}

- (float) distance:(SpaceObject *)other{
	return sqrt( sqr(self->x-other->x) + sqr(self->z-other->z) + sqr(self->y-other->y) );
}

- (void) localToGlobal{
	float _theta = pitch*pi/180;
	float phi = roll*pi/180;
	float psi = angle*pi/180;
	float X, Y, Z;

	__debug
	X = _x*cos(_theta) - _y*sin(_theta);		// rotate theta
	Y = _x*sin(_theta) + _y*cos(_theta);
	_x = X;
	_y = Y;
	Y = _y*cos(phi) + _z*sin(phi);			// rotate phi
	Z = -_y*sin(phi) + _z*cos(phi);
	_y = Y;
	_z = Z;
	X = -_x*sin(psi) + _z*cos(psi);			// rotate yaw
	Z = _x*cos(psi) + _z*sin(psi);
	_x = X;
	_z = Z;
	_x += x;								// offset to current position
	_y += y;
	_z += z;
	__debug
}

- (void) globalToLocal{
	float _theta = pitch*pi/180;
	float phi = roll*pi/180;
	float psi = angle*pi/180;
	float X, Y, Z;
	
	__debug
	_x -= x;
	_y -= y;
	_z -= z;
	X = -_x*sin(psi) + _z*cos(psi);
	Z = _x*cos(psi) + _z*sin(psi);
	_x = X;
	_z = Z;
	Y = _y*cos(phi) - _z*sin(phi);
	Z = _y*sin(phi) + _z*cos(phi);
	_y = Y;
	_z = Z;
	X = _x*cos(_theta) + _y*sin(_theta);
	Y = -_x*sin(_theta) + _y*cos(_theta);
	_x = X;
	_y = Y;
	__debug
}

- (void) calcAngles{		// convert global direction vectors to angles
	float tX, tZ, a;

	if( rY == 1.0f ){
		angle = atan2( -fX, fZ )*180./pi;
		__debug
		roll = 90;
		pitch = 0;
	} else if( rY == -1.0f ){
		angle = atan2( -fX, fZ)*180./pi;
		__debug
		roll = -90;
		pitch = 0;
	} else if( rX != 0 ){
		a = sqrt(1.+rZ*rZ/rX/rX);
		tX = -rZ/rX/a;
		tZ = 1.0f/a;
		a = tX*fX+tZ*fZ;
		if( a > 0 ){
			if( a > 1 )
				a = 1;
			angle = 180./pi*atan2( -tX, tZ );
			__debug
			pitch = 180./pi*acos(a);
			__debug
		} else {
			if( a < -1 )
				a = -1;
			angle = 180./pi*atan2( tX, -tZ );
			__debug
			pitch = 180.0f - 180./pi*acos(a);
			__debug
			tX *= -1.0f;
			tZ *= -1.0f;
		}
		if( uX*tX+uZ*tZ > 0 )
			pitch *= -1.0f;
		// rotate right 90¡ to find roll start location
		a = tZ;
		tZ = -tX;
		tX = a;
		a = rX*tX+rZ*tZ;
		if( a > 1 )
			a = 1;
		if( a < -1 )
			a = -1;
		roll = 180./pi*acos(a);
		__debug
		if( rY < 0 )
			roll *= -1.0f;
	} else {
		angle = 90;
		pitch = 0;
		roll = atan2( rY, rZ )*180./pi;
		__debug
	}
}

- (BOOL) visible{
	sys->_x = x;
	sys->_y = y;
	sys->_z = z;
	[sys globalToLocal];
	if( sys->viewStyle == 0 ){
		if( sys->_x < -size*1.9 )
			return NO;		
	} else if( sys->viewStyle == 2 ){
		if( sys->_x > size*1.9 )
			return NO;
	}
	return YES;
}

- (float) x{
	return x;
}

- (void) setX:(float)f{
	x = f;
}

- (float) y{
	return y;
}

- (void) setY:(float)f{
	y = f;
}

- (float) z{
	return z;
}

- (void) setZ:(float)f{
	z = f;
}

- (float) angle{
	return angle;
}

- (void) setAngle:(float)a{
	angle = a;
}

- (float) pitch{
	return pitch;
}

- (void) setPitch:(float)p{
	pitch = p;
}

- (float) roll{
	return roll;
}

- (void) setRoll:(float)r{
	roll = r;
}

- (float) angularAcceleration{
	return ANGULAR_ACCELERATION;
}

- (void) setAngularAcceleration:(float)a{
	ANGULAR_ACCELERATION = a;
}

- (float) maxAngularVelocity{
	return MAX_ANGULAR_VELOCITY;
}

- (void) setMaxAngularVelocity:(float)f{
	MAX_ANGULAR_VELOCITY = f;
}

- (float) maxVelocity{
	return MAX_VELOCITY;
}

- (void) setMaxVelocity:(float)v{
	MAX_VELOCITY = v;
}

- (int) mass{
	return mass;
}

- (void) setMass:(int)m{
	mass = m;
}

- (NSString *) name{
	return name;
}

- (void) setName:(NSString *)_name{
	[name release];
	name = [_name retain];
}

- (NSMutableDictionary *) flags{
	return flags;
}

@end
