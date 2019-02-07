//
//  Spaceship.m
//  Event Horizon
//
//  Created by user on Thu Dec 26 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "Spaceship.h"
#import "Controller.h"
#import "AI.h"
#import "Cargo.h"
#import "Weapon.h"
#import "Mod.h"
#import "Coord.h"
#import "ShieldSpot.h"

@implementation Spaceship

- (void) update{
	float jitterPitch, jitterRot;

	abSpeed = 0;
	throttle = YES;
	if( inertial )
		throttle = NO;
	if( left ){
		[self left];
		left = NO;
	}
	if( right ){
		[self right];
		right = NO;
	}
	if( forward ){
		[self forward];
		forward = NO;
		throttle = NO;
	} else if( throttledSpeed > 0 )
		[self forward];
	if( slow ){
		[self slow];
		slow = NO;
	}
	if( autopilot ){
		[self autopilot];
	}
	if( orbit ){
		[self toggleOrbit];
		orbit = NO;
	}
	if( hyper ){
		[self hyper];
	}
	if( land ){
		[self land];
		land = NO;
	}
	if( fire ){
		[self fire:YES];
		fire = NO;
	}
	if( fireSecondary ){
		[self fire:NO];
		fireSecondary = NO;
	}
	if( up ){
		[self up];
		up = NO;
	}
	if( down ){
		[self down];
		down = NO;
	}
	if( throttleUp ){
		[self throttle:YES];
		throttleUp = NO;
	}
	if( throttleDown ){
		[self throttle:NO];
		throttleDown = NO;
	}
	if( afterburner ){
		[self afterburner];
		afterburner = NO;
	}
	
	if( centerOfRotation )		// if we're orbiting...
		[self orbit];

	x += speedx*sys->FACTOR;
	z += speedz*sys->FACTOR;
	y += speedy*sys->FACTOR;
	
	[self collide:sys->planets];
	[self collide:sys->ships];
	[self collide:sys->asteroids];

	if( deltaRot > MAX_ANGULAR_VELOCITY )
		deltaRot = MAX_ANGULAR_VELOCITY;
	else if( deltaRot < -MAX_ANGULAR_VELOCITY )
		deltaRot = -MAX_ANGULAR_VELOCITY;
	if( deltaPitch > MAX_ANGULAR_VELOCITY )					// handle pitching motion
		deltaPitch = MAX_ANGULAR_VELOCITY;
	else if( deltaPitch < -MAX_ANGULAR_VELOCITY )
		deltaPitch = -MAX_ANGULAR_VELOCITY;

	if( abSpeed != 0 ){
		if( sqr(speedx)+sqr(speedz)+sqr(speedy) > sqr(MAX_VELOCITY+abSpeed) ){
			float vel = sqrt(sqr(speedx)+sqr(speedy)+sqr(speedz));
			speedx *= (MAX_VELOCITY+abSpeed)/vel;
			speedy *= (MAX_VELOCITY+abSpeed)/vel;
			speedz *= (MAX_VELOCITY+abSpeed)/vel;
		}
	} else if( sqr(speedx)+sqr(speedz)+sqr(speedy) > (throttle ? sqr(throttledSpeed) : sqr(MAX_VELOCITY)) ){
		float vel = sqrt(sqr(speedx)+sqr(speedz)+sqr(speedy));
		float speed;
		if( vel > (throttle ? throttledSpeed : MAX_VELOCITY) + ACCELERATION*sys->FACTOR )
			speed = vel-1.1*ACCELERATION*sys->FACTOR;
		else
			speed = throttle ? throttledSpeed : MAX_VELOCITY;
		speedx = speedx*speed/vel;
		speedz = speedz*speed/vel;
		speedy = speedy*speed/vel;
	} else if( throttle && sqr(speedx)+sqr(speedz)+sqr(speedy) < sqr(throttledSpeed) ){
		[self forward];
	}

	// handle jitter
	jitterPitch = jitter*(0.5-random()*1.0/RAND_MAX);
	jitterRot = jitter*(0.5-random()*1.0/RAND_MAX);
	jitter = jitter*exp(-4.6*sys->FACTOR);
	
	fX = cos(deltaRot*pi/180.*sys->FACTOR + jitterRot);		// handle angle
	fY = 0;
	fZ = -sin(deltaRot*pi/180.*sys->FACTOR + jitterRot);
	rX = sin(deltaRot*pi/180.*sys->FACTOR + jitterRot);
	rY = 0;
	rZ = cos(deltaRot*pi/180.*sys->FACTOR + jitterRot);		// end angle
	fY = sin(deltaPitch*pi/180.*sys->FACTOR + jitterPitch);	// handle pitch
	_x = cos(deltaPitch*pi/180.*sys->FACTOR + jitterPitch);		// normalize
	fX *= _x;
	fZ *= _x;						// end pitch
	uX = -fY*rZ;					// cross product
	uY = rZ*fX-fZ*rX;
	uZ = rX*fY-fX*rY;

	_x = rX;
	_y = rY;
	_z = rZ;
	[self localToGlobal];
	rX = _x - x;
	rY = _y - y;
	rZ = _z - z;
	_x = fX;
	_y = fY;
	_z = fZ;
	[self localToGlobal];
	fX = _x - x;
	fY = _y - y;
	fZ = _z - z;
	_x = uX;
	_y = uY;
	_z = uZ;
	[self localToGlobal];
	uX = _x - x;
	uY = _y - y;
	uZ = _z - z;
	[self calcAngles];
	
	if( state == ALIVE ){
		shields += rechargeRate*sys->FACTOR;
		if( shields > maxShield )
			shields = maxShield;
	}
}

- (void) left{
	deltaRot += ANGULAR_ACCELERATION*sys->FACTOR;
	if( deltaRot < 0 )
		deltaRot += MAX_ANGULAR_VELOCITY*sys->FACTOR;
}

- (void) right{
	deltaRot -= ANGULAR_ACCELERATION*sys->FACTOR;
	if( deltaRot > 0 )
		deltaRot -= MAX_ANGULAR_VELOCITY*sys->FACTOR;
}

- (void) forward{
	_x = ACCELERATION*sys->FACTOR;
	_y = 0;
	_z = 0;
	[self localToGlobal];
	__debug
	speedx += _x - x;
	speedz += _z - z;
	speedy += _y - y;
}

- (void) slow{
	throttledSpeed = 0;
}

- (void) throttle:(BOOL)_throttleUp{
	if( _throttleUp ){
		throttledSpeed += MAX_VELOCITY*sys->FACTOR*0.333;
		if( throttledSpeed > MAX_VELOCITY )
			throttledSpeed = MAX_VELOCITY;
	} else {
		throttledSpeed -= MAX_VELOCITY*sys->FACTOR*0.333;
		if( throttledSpeed < 0 )
			throttledSpeed = 0;
	}
}

- (void) toggleOrbit{
	if( curPlanet && curPlanet != centerOfRotation )
		[self setOrbit:curPlanet];
	else
		centerOfRotation = NULL;
}

- (void) orbit{
	float distanceSquared = sqr(x-centerOfRotation->x)+sqr(z-centerOfRotation->z)+sqr(y-centerOfRotation->y);
	float a = sys->GRAVITY*centerOfRotation->mass/distanceSquared/75;
	float _theta = atan2(centerOfRotation->z-z, centerOfRotation->x-x);
	float phi = atan2(centerOfRotation->y-y, sqrt(sqr(x-centerOfRotation->x)+sqr(z-centerOfRotation->z)));
	speedx += a*sys->FACTOR*cos(_theta)*cos(phi);
	speedz += a*sys->FACTOR*sin(_theta)*cos(phi);
	speedy += a*sys->FACTOR*sin(phi);
}

- (void) autopilot{		// point at selection
	SpaceObject *temp = nil;
	float deltaAngle;
	
	if( selection )
		temp = selection;
	else if( curPlanet )
		temp = curPlanet;
	if( temp ){
		_x = temp->x;
		_y = temp->y;
		_z = temp->z;
		[self globalToLocal];
		__debug
		if( _z < 0 )
			left = YES;
		else
			right = YES;
		if( _y > 0 )
			up = YES;
		else
			down = YES;
		if( _x*_x / (_x*_x + _y*_y + _z*_z) > .999 ){
			deltaPitch /= 2;
			deltaAngle /= 2;
		}
	} else {
		autopilot = NO;
	}
}

- (void) hyper{
	hyperTime += sys->FACTOR;
	if( hyperTime > 5.0f ){
		state = DEAD;
		if( sys->selection == self ){
			sys->selection = NULL;
			sys->shipIndex = -1;
		}
		[sys->enemies removeObject:self];
	}
}

- (void) land{
}

- init{
	self = [super init];
	texNum = 1;
	texture = calloc( 1, sizeof( GLuint ) );
	weapons = [[NSMutableArray alloc] initWithCapacity:5];
	mods = [[NSMutableArray alloc] initWithCapacity:5];
	enemies = [[NSMutableArray alloc] initWithCapacity:1];
	secondary = [[NSMutableArray alloc] initWithCapacity:1];
	secondaryIndex = -1;
	escorts = [[NSMutableArray alloc] init];
	compensation = 1;
	shieldSpots = [[NSMutableArray alloc] init];
	jitter = 0;
	inertial = NO;
	return self;
}

- (void) dealloc{
	free( texture );
	[weapons release];
	[mods release];
	[enemies release];
	[secondary release];
	[escorts release];

	[super dealloc];
}

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"ships"];
	Spaceship *ship = [[Spaceship alloc] init];

	if( [[[dictionary objectForKey:@"Data"] pathExtension] isEqualToString:@"obj"] || [[[dictionary objectForKey:@"Data"] pathExtension] isEqualToString:@"OBJ"] )
		[ship loadOBJ:[dictionary objectForKey:@"Data"]];
	else
		[ship load:[[NSFileHandle fileHandleForReadingAtPath:[dictionary objectForKey:@"Data"]] availableData] withTexture:[dictionary objectForKey:@"PicturePath"]];
	ship->name = [[dictionary objectForKey:@"Name"] copy];
	ship->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"ships", [dictionary objectForKey:@"ID"]];
	ship->ANGULAR_ACCELERATION = [[dictionary objectForKey:@"AngularAcceleration"] floatValue];
	ship->MAX_ANGULAR_VELOCITY = [[dictionary objectForKey:@"MaxAngularVelocity"] floatValue];
	ship->thrust = [[dictionary objectForKey:@"Thrust"] floatValue];
	ship->MAX_VELOCITY = [[dictionary objectForKey:@"MaxSpeed"] floatValue];
	ship->mass = [[dictionary objectForKey:@"Mass"] intValue];
	ship->size = [[dictionary objectForKey:@"Size"] floatValue];
	ship->cargoSpace = [[dictionary objectForKey:@"CargoSpace"] intValue];
	ship->modSpace = [[dictionary objectForKey:@"ModSpace"] intValue];
	if( [[dictionary objectForKey:@"Start"] boolValue] ){
		NSMutableDictionary *start = [Controller ensurePlugin:@"default" path:@"ships"];
		[start setObject:ship forKey:@"start"];
	}
	ship->flags = [[dictionary objectForKey:@"Flags"] mutableCopyWithZone:nil];
	ship->price = [[dictionary objectForKey:@"Price"] intValue];
	ship->description = [[dictionary objectForKey:@"Description"] copy];
	ship->initData = [dictionary objectForKey:@"Mods"];
	ship->shields = [[dictionary objectForKey:@"Shields"] intValue];
	ship->maxShield = ship->shields;
	ship->armor = [[dictionary objectForKey:@"Armor"] intValue];
	ship->maxArmor = ship->armor;
	ship->fuel = [[dictionary objectForKey:@"Fuel"] intValue];
	ship->maxFuel = ship->fuel;
	ship->defaultAI = [[dictionary objectForKey:@"DefaultAI"] copy];
	ship->primaryStraight = [dictionary objectForKey:@"Hardpoints"];
	ship->cargo = [dictionary objectForKey:@"Cargo"];
	ship->flagRequirements = [[dictionary objectForKey:@"FlagRequirements"] copy];
	if( [dictionary objectForKey:@"RechargeTime"] )
		ship->rechargeRate = ship->maxShield/[[dictionary objectForKey:@"RechargeTime"] floatValue];
	if( [dictionary objectForKey:@"FramePath"] )
		ship->frameFile = [[NSString alloc] initWithFormat:@"%@/%@", [[NSFileManager defaultManager] currentDirectoryPath], [dictionary objectForKey:@"FramePath"]];
	if( [dictionary objectForKey:@"Replace"] ){
		NSArray *temp = [[dictionary objectForKey:@"Replace"] componentsSeparatedByString:@"."];
		NSMutableDictionary *d = [Controller ensurePlugin:[temp objectAtIndex:0] path:@"ships"];
		[d setObject:ship forKey:[temp objectAtIndex:2]];
		ship->ID = [dictionary objectForKey:@"Replace"];
	} else
		[plugin setObject:ship forKey:[dictionary objectForKey:@"ID"]];
}

- (void) fire:(BOOL)primary{
	if( primary ){	// fire all primary weapons
		int i;

		for( i = 0; i < [weapons count]; i++ ){
			Weapon *weapon = [weapons objectAtIndex:i];
			if( primary && weapon->isPrimary ){
				if( sys->t - weapon->lastShot >= weapon->reload ){
					[weapon newInstanceFrom:self target:selection];
					weapon->lastShot = sys->t + (((float)random())/RAND_MAX/5.0f-.1f)*weapon->reload;
				}
			}
		}
	} else if( secondaryIndex != -1 ){		// fire selected secondary weapon
		Weapon *weapon = [self hasMod:((Mod*)[secondary objectAtIndex:secondaryIndex])->ammo];
		if( weapon && weapon->count > 0 && sys->t - weapon->lastShot >= weapon->reload ){
			[weapon newInstanceFrom:self target:selection];
			weapon->lastShot = sys->t + (((float)random())/RAND_MAX/5.-.1)*weapon->reload;
			weapon->count--;
			modSpace += weapon->mass;
			mass -= weapon->mass;
			ACCELERATION = thrust/(mass*compensation);
			if( weapon->count == 0 )
				[self->weapons removeObject:weapon];
		}
	}
}

- (void) up{
	deltaPitch += ANGULAR_ACCELERATION*sys->FACTOR;
	if( deltaPitch < 0 )
		deltaPitch += MAX_ANGULAR_VELOCITY*sys->FACTOR;
}

- (void) down{
	deltaPitch -= ANGULAR_ACCELERATION*sys->FACTOR;
	if( deltaPitch > 0 )
		deltaPitch -= MAX_ANGULAR_VELOCITY*sys->FACTOR;
}

- (void) afterburner{
	int i;
	for( i = 0; i < [mods count]; i++ ){
		Mod *m = [mods objectAtIndex:i];
		if( m->modType == AFTERBURNER ){
			abSpeed += m->value;
#pragma mark Not quite working
			fuel -= m->value2*sys->FACTOR;
			[self forward];
		}
	}
}

- (void) finalize{
	if( primaryStraight ){
		NSDictionary *hardPoints = (NSDictionary *)primaryStraight;
		NSEnumerator *enumerator = [hardPoints keyEnumerator];
		id key;
		NSMutableArray *array, *temp, *temp2;

		primaryStraight = nil;
		while( (key = [enumerator nextObject]) ){
			int i;
			if( [key isEqualToString:@"PrimaryStraight"] ){
				primaryStraight = [[NSMutableArray alloc] initWithCapacity:1];
				array = primaryStraight;
			}
			else if( [key isEqualToString:@"PrimaryTurret"] ){
				primaryTurret = [[NSMutableArray alloc] initWithCapacity:1];
				array = primaryTurret;
			}
			else if( [key isEqualToString:@"SecondaryStraight"] ){
				secondaryStraight = [[NSMutableArray alloc] initWithCapacity:1];
				array = secondaryStraight;
			}
			else if( [key isEqualToString:@"SecondaryTurret"] ){
				secondaryTurret = [[NSMutableArray alloc] initWithCapacity:1];
				array = secondaryTurret;
			}
			temp = (NSArray *)[hardPoints objectForKey:key];
			for( i = 0; i < [temp count]; i++ ){
				Coord *c = [[Coord alloc] init];
				temp2 = (NSArray *)[temp objectAtIndex:i];
				c->x = [[temp2 objectAtIndex:0] floatValue];
				c->y = [[temp2 objectAtIndex:1] floatValue];
				c->z = [[temp2 objectAtIndex:2] floatValue];
				[array addObject:c];
			}
		}
	}
}

- (id) newInstance{
	Class MyClass = NSClassFromString(defaultAI);
	AI *ai = [[MyClass alloc] initWithShip:self];
	ai->state = ALIVE;
	
	return ai;
}

- (void) addMod:(id)mod{
	NSMutableArray *array;

	if( [mod isKindOfClass:[Weapon class]] )
		array = weapons;
	else if( [mod isKindOfClass:[Mod class]]  ){
		Mod *m = mod;
		array = mods;
		if( m->modType == COMPENSATOR )				// need to handle removal
			compensation *= m->value;
		else if( m->modType == FUELTANK ){
			maxFuel += m->value;
		}
	}
	if( array ){
		int i;
		BOOL already = NO;
		for( i = 0; i < [array count]; i++ ){
			if( [[[array objectAtIndex:i] ID] isEqualToString:[mod ID]] ){
				[[array objectAtIndex:i] incrementCount];
				already = YES;
				break;
			}
		}
		if( !already ){
			mod = [mod shipInstance];
			[array addObject:mod];
			if( array == mods ){
				Mod *m = mod;
				if( m->ammo )
					[secondary addObject:mod];
			}
		}
		mass += [mod mass];
		ACCELERATION = thrust/(mass*compensation);
		modSpace -= [mod mass];		
	}
}

- hasMod:(id)mod{
	NSArray *array;
	if( !mod )
		return nil;
	if( [mod isKindOfClass:[Weapon class]] )
		array = weapons;
	else if( [mod isKindOfClass:[Mod class]] )
		array = mods;
	if( array ){
		int i;
		for( i = 0; i < [array count]; i++ ){
			if( [[mod ID] isEqualToString:[[array objectAtIndex:i] ID]] )
				return [array objectAtIndex:i];
		}
	}
	return nil;
}

- (void) hitBy:(Weapon *)weapon{
	float normal;
    if( weapon->target == self ){
        [self addEnemy:weapon->source];
    }
	_x = weapon->x;
	_y = weapon->y;
	_z = weapon->z;
	[self globalToLocal];
	normal = sqrt(_x*_x + _y*_y + _z*_z);
	_x *= size/normal;
	_y *= size/normal;
	_z *= size/normal;
	[shieldSpots addObject:[[ShieldSpot alloc] initAtX:_x y:_y z:_z time:sys->t size:weapon->force/10.0f]];
	jitter += weapon->force/(float)mass;
}

- (BOOL) addEnemy:(Spaceship *)enemy{
    if( ![enemies containsObject:enemy] ){
        [enemies addObject:enemy];
        [enemy->enemies addObject:self];
        return YES;
    }
    return NO;
}

- (void) bracket{
	glColor3f( 0, 1, 0 );
	if( [sys->enemies containsObject:self] )
		glColor3f( 1, 0, 0 );
	else if( escortee == sys )
		glColor3f( 1, 1, 0 );
	if( state == DISABLED )
		glColor3f( 0.5, 0.5, 0.5 );
	[super bracket];
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Spaceship: %@>", self->ID];
}

@end
