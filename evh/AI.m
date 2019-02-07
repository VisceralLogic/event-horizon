//
//  AI.m
//  Event Horizon
//
//  Created by user on Tue Jan 07 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "AI.h"
#import "Controller.h"
#import "Solarsystem.h"
#import "Cargo.h"

@class ShieldSpot;

@implementation AI

- initWithShip:(Spaceship *)ship{
	NSEnumerator *keys;
	id key;
	NSDictionary *temp = (NSDictionary *)ship->cargo;

	self = [self init];

	name = ship->name;
	MAX_VELOCITY = ship->MAX_VELOCITY;
	ANGULAR_ACCELERATION = ship->ANGULAR_ACCELERATION;
	MAX_ANGULAR_VELOCITY = ship->MAX_ANGULAR_VELOCITY;
	texture = ship->texture;
	mass = ship->mass;
	size = ship->size;
	thrust = ship->thrust;
	cargoSpace = ship->cargoSpace;
	modSpace = ship->modSpace;
	money = ship->price/4*random()/RAND_MAX;
	keys = [ship->initData keyEnumerator];
	while( key = [keys nextObject] ){
		int num = [[ship->initData objectForKey:key] intValue];
		int i;
		for( i = 0; i < num; i++ ){
			[self addMod:[Controller componentNamed:key]];
		}
	}
	shields = ship->shields;
	maxShield = ship->shields;
	armor = ship->armor;
	maxArmor = ship->armor;
	fuel = ship->fuel;
	maxFuel = ship->fuel;
	primaryStraight = ship->primaryStraight;
	primaryTurret = ship->primaryTurret;
	secondaryStraight = ship->secondaryStraight;
	secondaryTurret = ship->secondaryTurret;
	cargo = [[NSMutableArray alloc] initWithCapacity:1];
	if( temp ){
		Cargo *c = [[[Cargo alloc] init] autorelease];
		c->name = [[temp allKeys] objectAtIndex:0];
		c->size = [[temp objectForKey:[[temp allKeys] objectAtIndex:0]] intValue];
		[cargo addObject:c];
		cargoSpace -= c->size;
		mass += c->size;
	}
	ACCELERATION = thrust/(mass*compensation);
	listNum = ship->listNum;
	list = ship->list;
	texNum = ship->texNum;
	obj = ship->obj;
	rechargeRate = ship->rechargeRate;
	
	return self;
}

- (void) dealloc{
	[cargo release];
	
	[super dealloc];
}

- (void) update{
	if( shields <= 0 && armor <= 0 ){
		if( startExplode == 0 ){
			startExplode = sys->t;
		} else if( sys->t - startExplode >= 5 ){
			state = DEAD;
			if( sys->selection == self ){
				sys->selection = nil;
				sys->shipIndex = -1;
			}
		}
		[super update];
		return;
	} else if( armor <= maxArmor/4 ){	// DISABLED
		state = DISABLED;
		slow = YES;
		autopilot = NO;
		orbit = NO;
		centerOfRotation = NULL;
		[super update];
		return;
	}
	
	switch( goal ){
		case DO_LAND:
			[self doLand];
			break;
		case DO_LEAVE:
			[self doLeave];
			break;
		case DO_ATTACK:
			[self doAttack];
			break;
		case DO_LOITER:
			[self doLoiter];
			break;
		case DO_ESCORT:
			[self doEscort];
			break;
		default:
			goal = DO_LEAVE;
	}
	
	[self navigate:sys->planets];
	
	[super update];
}

- (void) land{
	if( curPlanet ){
		float d = sqrt( sqr(x-curPlanet->x) + sqr(z-curPlanet->z) + sqr(y-curPlanet->y) );
		if( d <= curPlanet->size + size + 0.15 ){
			state = DEAD;
			if( sys->selection == self ){
				sys->selection = NULL;
				sys->shipIndex = -1;
			}
		}
	}
}

- (void) draw{
	if( ![self visible] )
		return;
	[self position];
	glRotatef( 90, 0, 1, 0 );					// rotate to its angle
	glScalef( size, size, size );					// scale to size
	if( shields <= 0 && armor <= 0 ){
		if( sys->t - startExplode < 2.5 )
			[self drawExplosionSize:sin((sys->t-startExplode)*pi/2.5) atX:0.5 z:0];
		if( sys->t - startExplode < 4 )
			[self drawExplosionSize:sin((sys->t-startExplode-1.5)*pi/2.5) atX:0 z:-0.5];
		[self drawExplosionSize:1.5*sin((sys->t-startExplode-2)*pi/3) atX:0 z:0];
		if( sys->t - startExplode > 2.5 )
			return;
	}
	glBindTexture( GL_TEXTURE_2D, texture[0] );
	[self drawObject];
}

- (void) drawExplosionSize:(float)_size atX:(float)x z:(float)z{
	glDisable( GL_LIGHTING );
	glBindTexture( GL_TEXTURE_2D, sys->texture[FIRE_TEXTURE] );
	glTranslatef( x, 0, z );
	glRotatef( 90, 1, 0, 0 );
	glRotatef( 50*sys->t, 0, 0, 1 );
	gluSphere( sphereObj, _size, 20, 20 );
	glRotatef( -50*sys->t, 0, 0, 1 );
	glRotatef( -90, 1, 0, 0 );
	glTranslatef( -x, 0, -z );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glEnable( GL_LIGHTING );
}

- (void) navigate:(NSArray *)_objects{
	NSEnumerator *obEnum = [_objects objectEnumerator];
	SpaceObject *ob;
	while( ob = [obEnum nextObject] ){		// for each object
		float _distance, dotProduct;
		
		if( ob == target )
			return;
		_distance = sqrt( sqr(ob->x-x) + sqr(ob->z-z) );
		dotProduct = speedx*(ob->x-x) + speedz*(ob->z-z);
		if( (dotProduct > 0) && (_distance < 5+size+ob->size) && sqr(ob->y-y) < sqr(size+ob->size) ){
		// we're approaching it and it is less than 5 units distance
			float approach = dotProduct/_distance;	// length of velocity vector point at ob
			float theta = asin( approach/sqrt(speedx*speedx+speedz*speedz) ) + pi*angle/180;
			
			_distance -= size+ob->size;
			_distance = 1. - 1./_distance/_distance;
			if( _distance < 0.1 )
				_distance = 0.1;
			_distance *= MAX_VELOCITY;
			if( approach > _distance ){
				//speedx -= (approach-distance)*cos( theta );
				//speedz -= (approach-distance)*sin( theta );
				slow = YES;
				if( y >= ob->y )
					up = YES;
				else
					down = YES;
			}
		}
	}
}

- (void) doLand{
	if( !target ){
		float num = ((float)random())/RAND_MAX*[sys->planets count];
		target = [sys->planets objectAtIndex:(int)num];
		curPlanet = target;
	}

	autopilot = YES;
	distance = sqrt(sqr(x-curPlanet->x) + sqr(z-curPlanet->z)) - curPlanet->size - size;
	if( !centerOfRotation )
		orbit = YES;
	forward = YES;
	if( distance < 1.5 ){
		land = YES;
	} else {
		if( sqr( deltaRot ) > sqr(MAX_ANGULAR_VELOCITY/2) )
			slow = YES;
		else
			forward = YES;
	}
}

- (void) doLeave{
	if( !target ){
		float num = ((float)random())/RAND_MAX*[sys->system->links count];
		target = [sys->system->links objectAtIndex:(int)num];
		angle = 180./pi*atan2( ((Solarsystem*)target)->z-sys->system->z, ((Solarsystem*)target)->x-sys->system->x );
	}

	forward = YES;
	if( x*x+z*z+y*y > jumpDistance || [sys->system->planets count] == 0 ){
		hyper = YES;
	}
}

- (void) doAttack{
	float dist;
	
	autopilot = YES;
	if( sqr( deltaRot ) > sqr(MAX_ANGULAR_VELOCITY/2) )
		slow = YES;
	else
		forward = YES;
	
	dist =  sqrt(sqr(x-target->x)+sqr(z-target->z)+sqr(y-target->y));
	if( dist < 5 ){
		fire = YES;
		if( dist < 2 )
			slow = YES;
	}
	if( ++secondaryIndex >= [secondary count] )
		secondaryIndex = -1;
	fireSecondary = YES;
}

- (void) doLoiter{
	slow = YES;
}

- (void) doEscort{
	selection = escortee;
	autopilot = YES;
	if( sqr(x-escortee->x)+sqr(z-escortee->z) > sqr(5*size+escortee->size) )
		forward = YES;
}

@end
