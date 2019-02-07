//
//  Weapon.m
//  Event Horizon
//
//  Created by user on Sat Mar 01 2003.
//  Copyright (c) 2003 Paul Dorman. All rights reserved.
//

#import "Weapon.h"
#import "Coord.h"
#import "SoundManager.h"
#import "Texture.h"
#import "ControllerThreading.h"

@implementation Weapon

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"weapons"];
	Weapon *weap = [[Weapon alloc] init];
	weap->initData = [[NSMutableDictionary alloc] init];

	if( [[[dictionary objectForKey:@"Data"] pathExtension] isEqualToString:@"obj"] || [[[dictionary objectForKey:@"Data"] pathExtension] isEqualToString:@"OBJ"] )
		[weap loadOBJ:[dictionary objectForKey:@"Data"]];
	else if( [dictionary objectForKey:@"Data"] )
		[weap load:[[NSFileHandle fileHandleForReadingAtPath:[dictionary objectForKey:@"Data"]] availableData] withTexture:[dictionary objectForKey:@"PicturePath"]];
	weap->name = [[dictionary objectForKey:@"Name"] copy];
	weap->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"weapons", [dictionary objectForKey:@"ID"]];
	weap->ANGULAR_ACCELERATION = [dictionary objectForKey:@"AngularAcceleration"] ? [[dictionary objectForKey:@"AngularAcceleration"] floatValue] : 0;
	weap->MAX_ANGULAR_VELOCITY = [dictionary objectForKey:@"MaxAngularVelocity"] ? [[dictionary objectForKey:@"MaxAngularVelocity"] floatValue] : 0;
	weap->ACCELERATION = [dictionary objectForKey:@"Acceleration"] ? [[dictionary objectForKey:@"Acceleration"] floatValue] : 0;
	weap->MAX_VELOCITY = [dictionary objectForKey:@"MaxSpeed"] ? [[dictionary objectForKey:@"MaxSpeed"] floatValue] : 0;
	weap->mass = [[dictionary objectForKey:@"Mass"] intValue];
	weap->size = [[dictionary objectForKey:@"Size"] floatValue];
	weap->life = [[dictionary objectForKey:@"Life"] floatValue];
	weap->isPrimary = [[dictionary objectForKey:@"Primary"] boolValue];
	weap->flags = [[dictionary objectForKey:@"Flags"] mutableCopyWithZone:nil];
	weap->description = [[dictionary objectForKey:@"Description"] copy];
	weap->price = [[dictionary objectForKey:@"Price"] intValue];
	weap->reload = [[dictionary objectForKey:@"Reload"] floatValue];
	weap->force = [[dictionary objectForKey:@"Force"] intValue];
	weap->max = [[dictionary objectForKey:@"Max"] intValue];
	weap->inAcc = [dictionary objectForKey:@"Inaccuracy"] ? [[dictionary objectForKey:@"Inaccuracy"] floatValue] : 0;
	if( [[dictionary objectForKey:@"LaunchStyle"] isEqualToString:@"STRAIGHT"] )
		weap->launchStyle = STRAIGHT;
	else if( [[dictionary objectForKey:@"LaunchStyle"] isEqualToString:@"TURRET"] )
		weap->launchStyle = TURRET;
	else if( [[dictionary objectForKey:@"LaunchStyle"] isEqualToString:@"SPACESHIP"] )
		weap->launchStyle = SPACESHIP;
	else if( [[dictionary objectForKey:@"LaunchStyle"] isEqualToString:@"BEAM"] )
		weap->launchStyle = BEAM;
	weap->proximity = 0;
	if( [dictionary objectForKey:@"Proximity"] )
		weap->proximity = [[dictionary objectForKey:@"Proximity"] floatValue];
	if( !weap->isPrimary )
		[weap->initData setObject:[dictionary objectForKey:@"Launcher"] forKey:@"Launcher"];
	if( [dictionary objectForKey:@"Sound"] )
		weap->sound = [SoundManager registerSound:[[NSURL fileURLWithPath:[dictionary objectForKey:@"Sound"]] path]];
	if( [dictionary objectForKey:@"Spaceship"] )
		[weap->initData setObject:[dictionary objectForKey:@"Spaceship"] forKey:@"Spaceship"];
	glGenTextures( 1, &weap->modPic );
	[Texture loadTexture:[dictionary objectForKey:@"ModPicture"] into:&weap->modPic withWidth:nil height:nil]; 
	weap->flagRequirements = [dictionary objectForKey:@"FlagRequirements"];
	weap->proxMass = [dictionary objectForKey:@"ProximityMass"] ? [[dictionary objectForKey:@"ProximityMass"] intValue] : 0;
	weap->disableLighting = [dictionary objectForKey:@"DisableLighting"] ? [[dictionary objectForKey:@"DisableLighting"] boolValue] : NO;
	if( [dictionary objectForKey:@"Replace"] ){
		NSArray *temp = [[dictionary objectForKey:@"Replace"] componentsSeparatedByString:@"."];
		NSMutableDictionary *d = [Controller ensurePlugin:[temp objectAtIndex:0] path:@"weapons"];
		[d setObject:weap forKey:[temp objectAtIndex:2]];
		weap->ID = [dictionary objectForKey:@"Replace"];
	} else
		[plugin setObject:weap forKey:[dictionary objectForKey:@"ID"]];
}

- shipInstance{
	Weapon *weap = [[Weapon alloc] init];
	weap->tri = tri;
	weap->quad = quad;
	weap->poly = poly;
	weap->texture = texture;
	weap->listNum = listNum;
	weap->list = list;
	weap->texNum = texNum;
	weap->obj = obj;
	weap->MAX_VELOCITY = MAX_VELOCITY;
	weap->ANGULAR_ACCELERATION = ANGULAR_ACCELERATION;
	weap->MAX_ANGULAR_VELOCITY = MAX_ANGULAR_VELOCITY;
	weap->ACCELERATION = ACCELERATION;
	weap->size = size;
	weap->life = life;
	weap->reload = reload;
	weap->isPrimary = isPrimary;
	weap->force = force;
	weap->name = name;
	weap->count = 1;
	weap->ID = self->ID;
	weap->max = max;
	weap->sound = sound;
	weap->mass = mass;
	weap->proximity = proximity;
	weap->inAcc = inAcc;
	weap->launchStyle = launchStyle;
	weap->launcher = launcher;
	weap->lastHardpoint = -1;
	weap->listNum = listNum;
	weap->spaceship = spaceship;
	weap->proxMass = proxMass;
	weap->disableLighting = disableLighting;
	
	return weap;
}

- (void) newInstanceFrom:(Spaceship *)_source target:(Spaceship *)_target{
	Weapon *weap = [[Weapon alloc] init];
	NSArray *hardpoint = nil;

// need to add trigonometry to account for extra y displacement at launch during pitched firing
// right now it just fires based on horizontal circle to exit spaceship region

	source = _source;
	target = _target;
	
	if( isPrimary ){
		if( launchStyle == STRAIGHT )
			hardpoint = source->primaryStraight;
		else if( launchStyle == TURRET )
			hardpoint = source->primaryTurret;
	} else {
		if( launchStyle == STRAIGHT )
			hardpoint = source->secondaryStraight;
		else if( launchStyle == TURRET )
			hardpoint = source->secondaryTurret;
		else if( launchStyle == SPACESHIP ){
			AI *ai;
			Class AIClass = NSClassFromString(spaceship->defaultAI);
			ai = [[AIClass alloc] initWithShip:spaceship];
			ai->escortee = source;
			ai->x = source->x+(ai->size+source->size)*cos(source->angle*pi/180+pi/2);
			ai->z = source->z+(ai->size+source->size)*sin(source->angle*pi/180+pi/2);
			ai->y = source->y;
			ai->speedx = source->speedx;
			ai->speedz = source->speedz;
			ai->speedy = source->speedy;
			ai->angle = source->angle;
			ai->pitch = source->pitch;
			ai->roll = source->roll;
			ai->weaponID = self->ID;
			ai->gov = source->gov;
			[source->escorts addObject:ai];
			[sys->ships addObject:ai];
			return;
		}
	}
	if( launchStyle == BEAM )
		hardpoint = source->primaryTurret;
	if( hardpoint ){
		Coord *c;
		lastHardpoint++;
		if( lastHardpoint == [hardpoint count] )
			lastHardpoint = 0;
		c = (Coord *)[hardpoint objectAtIndex:lastHardpoint];
		source->_x = c->x*source->size;
		source->_z = c->z*source->size;
		source->_y = c->y*source->size;
	} else {
		source->_x = 0;
		source->_y = 0;
		source->_z = 0;
	}
	weap->source = source;
	weap->target = target;
	if( launchStyle == STRAIGHT || (launchStyle != SPACESHIP && !target) ){
		source->_x = size+sqrt(sqr(source->size)-sqr(source->_z));
		[source localToGlobal];
		weap->x = source->_x;
		weap->z = source->_z;
		weap->y = source->_y;
		weap->angle = source->angle;
		weap->pitch = source->pitch;
		weap->roll = source->roll;
	}
	else if( launchStyle == TURRET ){
		float _angle = atan2( target->z-source->z, target->x-source->x );
		weap->pitch = atan2( target->y-source->y, sqrt( sqr(target->x-source->x)+sqr(target->z-source->z) ) );
		weap->angle = 180.*_angle/pi - 90.;
		[source localToGlobal];
		weap->x = source->_x+(size+source->size)*cos(_angle);
		weap->z = source->_z+(size+source->size)*sin(_angle);
		weap->y = source->_y+(size+source->size)*sin(weap->pitch);
		weap->pitch *= 180./pi;
	}
	weap->angle +=  inAcc*sin(random());
	weap->pitch += inAcc*sin(random());
	weap->MAX_VELOCITY = MAX_VELOCITY;
	weap->ANGULAR_ACCELERATION = ANGULAR_ACCELERATION;
	weap->MAX_ANGULAR_VELOCITY = MAX_ANGULAR_VELOCITY;
	weap->ACCELERATION = ACCELERATION;
	weap->size = size;
	weap->life = life;
	weap->force = force;
	weap->proximity = proximity;
	weap->listNum = listNum;
	weap->list = list;
	weap->texNum = texNum;
	weap->obj = obj;
	weap->texture = texture;
	weap->proxMass = proxMass;
	weap->disableLighting = disableLighting;
	weap->launchStyle = launchStyle;
	if( launchStyle == BEAM ){
		weap->_x = weap->x - source->x;
		weap->_y = weap->y - source->y;
		weap->_z = weap->z - source->z;
	}
	[weaponLock lock];
	[sys->weaps addObject:weap];
	[weaponLock unlock];
	[SoundManager attachSound:sound toSource:weap];
}

- (void) update{
	if( launchStyle != BEAM ){
		if( target && ANGULAR_ACCELERATION != 0 ){	// we're guided
			float deltaAngle = atan2( target->z-z, target->x-x )*180/pi-90;
			deltaAngle -= angle;
			while( deltaAngle < 0 )	// get it between 0 and 360
				deltaAngle += 360;
			if( sqr(deltaAngle) < sqr(ANGULAR_ACCELERATION*sys->FACTOR) ){
				if( sqr(deltaRot) < sqr(ANGULAR_ACCELERATION*sys->FACTOR) ){
					angle += deltaAngle;
					deltaRot = 0;
				}
				else
					deltaRot /= 1+10*sys->FACTOR;
			} else {
				if( deltaAngle > 180 ){
					if( deltaRot > ANGULAR_ACCELERATION )
						deltaRot /= 1+10*sys->FACTOR;
					else
						deltaRot -= ANGULAR_ACCELERATION*sys->FACTOR;
				} else {
					if( deltaRot < -ANGULAR_ACCELERATION )
						deltaRot /= 1+10*sys->FACTOR;
					else
						deltaRot += ANGULAR_ACCELERATION*sys->FACTOR;
				}
				if( sqr(deltaRot) > sqr(MAX_ANGULAR_VELOCITY) ){
					deltaRot *= MAX_ANGULAR_VELOCITY/abs(deltaRot);
				}
			}
			angle += deltaRot*sys->FACTOR;
			if( angle > 360 )
				angle -= 360;
			else if( angle < 0 )
				angle += 360;
			roll = -deltaRot/6;
			pitch = 180*atan2(target->y-y, sqrt(sqr(target->x-x)+sqr(target->z-z)))/pi;
		}
		if( ACCELERATION != 0 ){
			_x = ACCELERATION*sys->FACTOR;
			_y = 0;
			_z = 0;
			[self localToGlobal];
			speedx += _x - x;
			speedz += _z - z;
			speedy += _y - y;
		} else {
			_x = MAX_VELOCITY;
			_y = 0;
			_z = 0;
			[self localToGlobal];
			speedx = _x - x;
			speedz = _z - z;
			speedy = _y - y;
		}
		if( sqr(speedx)+sqr(speedz)+sqr(speedy) > MAX_VELOCITY*MAX_VELOCITY ){
			float vel = sqrt(sqr(speedx)+sqr(speedz)+sqr(speedy));
			speedx *= MAX_VELOCITY/vel;
			speedz *= MAX_VELOCITY/vel;
			speedy *= MAX_VELOCITY/vel;
		}
		x += speedx*sys->FACTOR;
		z += speedz*sys->FACTOR;
		y += speedy*sys->FACTOR;		
	} else {
		// update position for BEAM weapon
	}
	[self collide:sys->ships];
	[self collide:sys->planets];
	[self collide:sys->asteroids];
	if( life < 0 )
		[sys->weaps removeObject:self];
	life -= sys->FACTOR;
}

- (void) draw{
	if( ![self visible] )
		return;
	glBindTexture( GL_TEXTURE_2D, texture[0] );
	[self position];
	//	glRotatef( pitch, 1, 0, 0 );
	glRotatef( 90, 0, 1, 0 );
	glScalef( size, size, size );
	if( disableLighting )
		glDisable( GL_LIGHTING );
	[self drawObject];
	if( disableLighting )
		glEnable( GL_LIGHTING );	
}

- (void) collide:(NSArray *)_objects{
	int i;
	float _distance;
	float prox = 0;
	float x1, y1, z1;
	int _count = [_objects count];

	_x = size;
	_y = 0;
	_z = 0;
	[self localToGlobal];
	x1 = _x;
	y1 = _y;
	z1 = _z;
	_x = -size;
	_y = 0;
	_z = 0;
	[self localToGlobal];
	for( i = 0; i < _count; i++ ){
		SpaceObject *sphere = [_objects objectAtIndex:i];
		BOOL _spaceship = [sphere isKindOfClass:[Spaceship class]];
		if( sphere == source )		// don't attack the ship you came from
			continue;
		if( _spaceship )
			prox = proximity;
		_distance = sqr(sphere->x-x) + sqr(sphere->z-z) + sqr(sphere->y-y);
		if( _distance > sqr(size+sphere->size+prox) )
			continue;		// way out of range
		if( _distance < sqr(sphere->size+prox) || 
			sqr(sphere->y-y1)+sqr(sphere->z-z1)+sqr(sphere->x-x1) < sqr(sphere->size+prox) || 
			sqr(sphere->y-_y)+sqr(sphere->z-_z)+sqr(sphere->x-_x) < sqr(sphere->size+prox) ){
			if( _spaceship ){			// hit a spaceship
				if( proxMass == 0 || sphere->mass >= proxMass ){		// heavy enough
					Spaceship *ship = (Spaceship *)sphere;
					ship->shields -= force;
					if( ship->shields < 0 ){	// shields down, attack armor
						ship->armor += ship->shields;
						ship->shields = 0;
						if( ship->armor < 0 )	// don't want it to go negative
							ship->armor = 0;
					}
					[ship hitBy:self];
					[sys->weaps removeObject:self];
				}
				if( source == sys ){
					Spaceship *s = (Spaceship *)sphere;
					if( target == s )
						[sys->govRecord setObject:[NSNumber numberWithInt:[[sys->govRecord objectForKey:s->gov->ID] intValue]-s->gov->shootPenalty] forKey:s->gov->ID];
				}
			} else		// hit a planet or asteroid
				[sys->weaps removeObject:self];
			break;
		}
	}
}

- (void) finalize{
	id object;
	if( object = [initData objectForKey:@"Launcher"] )
		launcher = [Controller componentNamed:object];
	if( object = [initData objectForKey:@"Spaceship"] )
		spaceship = [Controller componentNamed:object];
	[initData release];
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Weapon: %@>", self->ID];
}

- (int) price{
	return price;
}

- (int) mass{
	return mass;
}

- (NSString *) ID{
	return self->ID;
}

- (void) incrementCount{
	if( isPrimary )
		reload *= (float)count/(float)(count+1);
	count++;
}

- (int) count{
	return count;
}

- (int) max{
	return max;
}

- (NSString *) desc{
	return description;
}

- (GLuint) texture{
	return modPic;
}

- (NSString *) name{
	return name;
}

- (NSMutableDictionary *) flags{
	return flags;
}

- (NSString *) flagRequirements{
	return flagRequirements;
}

@end
