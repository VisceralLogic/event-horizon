//
//  SpaceObject.h
//  Event Horizon
//
//  Created by user on Wed Aug 07 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import <Carbon/carbon.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <StdIO.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#import "main.h"

#define __debug if( isnan(x) || isnan(y) || isnan(z) \
					|| isnan(speedx) || isnan(speedy) || isnan(speedz) \
					|| isnan(angle) || isnan(roll) || isnan(pitch) ) ;// [self crash];

@class Controller;
@class Planet;

@interface SpaceObject : NSObject {
	@public

	float MAX_ANGULAR_VELOCITY;		// maximum rate of rotation
	float ANGULAR_ACCELERATION;		// rate of increase of rotation
	float ACCELERATION;
	float MAX_VELOCITY;

	float angle, roll, pitch;		// angle in xz plane, about local z, about local x
	float deltaPitch, deltaRoll;	// pitch and roll angular velocity
	float x, y, z;					// cartesian coordinates
	float speedx, speedy, speedz;	// velocity vector
	float deltaRot;					// angular velocity
	
	GLuint *texture;				// array of gl textures
	int texNum;						// number of textures
	
	float size;						// radius from center
	int mass;						// mass of object
	
	SpaceObject *centerOfRotation;	// SpaceObject to orbit
	float distance;					// distance from centerOfRotation
	double w;						// angular velocity about centerOfRotation
	double theta;					// angle about centerOfRotation

	NSString *name;
	NSString *ID;					// used for storing player data
	id initData;					// used to store data between init and finalize
	NSString *description;			// information about this
	NSString *flagRequirements;		// flags planet must have for this to be sold there
	NSMutableDictionary *flags;	// each flag has its name as the key, and its value as the object
	float _x, _y, _z;				// used for rotating vectors from local to global
	float fX, fY, fZ;				// forward vector in global
	float rX, rY, rZ;				// right vector in global
	float uX, uY, uZ;				// up vector in global
}

+ (void) initialize;
- (void) loadTextures:(NSString **)files;
- (void) bracket;
- (void) setOrbit:(SpaceObject *)center;
- (void) collide:(NSArray *)objects;
- (void) bounce:(SpaceObject *)sphere;
- (void) position;
- (float) distance:(SpaceObject *)other;
- (void) localToGlobal;
- (void) globalToLocal;
- (void) calcAngles;
- (BOOL) visible;

// useful for scripting
- (float) x;
- (void) setX:(float)x;
- (float) y;
- (void) setY:(float)y;
- (float) z;
- (void) setZ:(float)z;
- (float) angularAcceleration;
- (void) setAngularAcceleration:(float)a;
- (float) maxAngularVelocity;
- (void) setMaxAngularVelocity:(float)w;
- (float) maxVelocity;
- (void) setMaxVelocity:(float)v;
- (int) mass;
- (void) setMass:(int)mass;
- (NSString *) name;
- (void) setName:(NSString *)name;
- (NSMutableDictionary *) flags;

@end

extern SpaceObject *SPACEOBJECT_ORIGIN;
extern Controller *sys;
extern GLUquadricObj *sphereObj;