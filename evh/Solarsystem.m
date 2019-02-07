//
//  System.m
//  Event Horizon
//
//  Created by user on Sat Sep 21 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "Solarsystem.h"
#import "AI.h"
#import "Controller.h"
#import "Asteroid.h"
#import "Type.h"
#import "Texture.h"

GLuint backdrop[4];
int backdropWidth[4];
int backdropHeight[4];

@implementation Solarsystem

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	int i;
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"systems"];
	Solarsystem *system = [[Solarsystem alloc] init];
	NSArray *temp = [dictionary objectForKey:@"Coords"];
	
	system->x = [[temp objectAtIndex:0] floatValue];
	system->z = [[temp objectAtIndex:1] floatValue];
	if( [[dictionary objectForKey:@"Start"] boolValue] ){
		NSMutableDictionary *start = [Controller ensurePlugin:@"default" path:@"systems"];
		[start setObject:system forKey:@"start"];
	}
	system->name = [[dictionary objectForKey:@"Name"] copy];
	system->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"systems", [dictionary objectForKey:@"ID"]];
	temp = [dictionary objectForKey:@"Planets"];
	system->planets = [[NSMutableArray alloc] initWithCapacity:[temp count]];
	system->initData = [[NSMutableArray alloc] initWithArray:temp];
	temp = [dictionary objectForKey:@"Links"];
	system->links = [[NSMutableArray alloc] initWithCapacity:[temp count]];
	[system->initData addObject:temp];
	[system->initData addObject:[dictionary objectForKey:@"Government"]];
	system->flags = [[dictionary objectForKey:@"Flags"] mutableCopyWithZone:nil];
	system->shipCount = [[dictionary objectForKey:@"ShipCount"] intValue];
	system->description = [[dictionary objectForKey:@"Description"] copy];
	system->asteroids = [[dictionary objectForKey:@"Asteroids"] intValue];
	[system->initData addObject:[dictionary objectForKey:@"ShipTypes"]];
	system->backdrops = [[dictionary objectForKey:@"Backdrops"] count];
	for( i = 0; i < system->backdrops; i++ ){
		NSDictionary *drops = [[dictionary objectForKey:@"Backdrops"] objectAtIndex:i];
		system->backdropAngle[i] = [[drops objectForKey:@"Angle"] intValue];
		system->backdropElev[i] = [[drops objectForKey:@"Height"] intValue];
		system->backdropPath[i] = [[[[NSFileManager defaultManager] currentDirectoryPath] stringByAppendingFormat:@"/%@", [drops objectForKey:@"Path"]] retain];
	}
	system->displayOnMap = NO;
	if( [dictionary objectForKey:@"Replace"] ){
		NSArray *temp = [[dictionary objectForKey:@"Replace"] componentsSeparatedByString:@"."];
		NSMutableDictionary *d = [Controller ensurePlugin:[temp objectAtIndex:0] path:@"systems"];
		[d setObject:system forKey:[temp objectAtIndex:2]];
		system->ID = [dictionary objectForKey:@"Replace"];
	} else
		[plugin setObject:system forKey:[dictionary objectForKey:@"ID"]];
}

- (void) finalize{
	int i, count = [initData count];
	NSMutableArray *temp;

//NSLog( @"Finalizing system: %@", name );
	for( i = 0; i < count-3; i++ ){	// load planets
//NSLog( @"\tadding planet: %@", [initData objectAtIndex:i] );
		[planets addObject:[Controller componentNamed:[initData objectAtIndex:i]]];
	}

	temp = [initData objectAtIndex:count-3];	// load links
	for( i = 0; i < [temp count]; i++ ){
		Solarsystem *s;
//NSLog( @"\tadding link: %@", [temp objectAtIndex:i] );
		s = [Controller componentNamed:[temp objectAtIndex:i]];
		if( ![links containsObject:s] )
			[links addObject:s];
		if( ![s->links containsObject:self] )
			[s->links addObject:self];
	}
	
	gov = [Controller componentNamed:[initData objectAtIndex:count-2]];
	
	temp = [initData objectAtIndex:count-1];
	types = [[Type alloc] init];
	types->data = [[NSMutableArray alloc] initWithCapacity:[temp count]];
	for( i = 0; i < [temp count]; i++ ){
		NSDictionary *dict = [temp objectAtIndex:i];
		TypeNode *node = [[TypeNode alloc] init];
		node->data = [Controller componentNamed:[dict objectForKey:@"TypeID"]];
		node->probability = [[dict objectForKey:@"Probability"] floatValue];
		[types->data addObject:node];
	}
	[initData release];
}

- (void) setUp{
	int i, j = 0;

	displayOnMap = YES;
	sys->selection = NULL;
	sys->shipIndex = -1;
	sys->curPlanet = NULL;
	sys->planetIndex = -1;
	[sys->enemies removeAllObjects];
	[sys->ships removeAllObjects];
	[sys->ships addObject:sys];
	[sys->weaps removeAllObjects];
	[sys->asteroids removeAllObjects];
	for( i = 0; i < [planets count]; i++ ){
		Planet *planet = [planets objectAtIndex:i];
		if( !planet->model ){
			[Texture loadTexture:planet->texFile into:&sys->planetTex[i] withWidth:nil height:nil];
			planet->texture[0] = sys->planetTex[i];
			if( planet->ringSize != 0 ){
				[Texture loadTexture:planet->ringTex into:&sys->planetTex[[planets count]+j] withWidth:nil height:nil];
				planet->texture[1] = sys->planetTex[[planets count]+j++];
			}
			if( planet->atmosSize != 0 ){
				[Texture loadTexture:planet->atmosTex into:&sys->planetTex[[planets count]+j] withWidth:nil height:nil];
				planet->texture[2] = sys->planetTex[[planets count]+j++];
			}
		}
	}
	if( sys->planets )
		[sys->planets release];	
	sys->planets = [[NSMutableArray alloc] initWithArray:planets];
	for( i = 0; i < shipCount; i++ ){
		AI *tempShip = [types newInstance];
		tempShip->x = 60.0f-(120.0f*random())/RAND_MAX;
		tempShip->z = 60.0f-(120.0f*random())/RAND_MAX;
		tempShip->y = 60.0f-(120.0f*random())/RAND_MAX;
		[sys->ships addObject:tempShip];
	}
	for( i = 0; i < asteroids; i++ ){
		[sys->asteroids addObject:[[Asteroid alloc] init]];
	}
	for( i = 0; i < backdrops; i++ ){
		[Texture loadTexture:backdropPath[i] into:&backdrop[i] withWidth:&backdropWidth[i] height:&backdropHeight[i]];
	}
	if( ![sys->systems containsObject:self->ID] )
		[sys->systems addObject:self->ID];
}

- (void) update{
	int i;
	
	if( sys->viewStyle == 1 )
		return;
	glDisable( GL_DEPTH_TEST );
	glColor3f( 1, 1, 1 );
	glLoadIdentity();
	if( sys->viewStyle == 0 ){
		glRotatef( -sys->pitch, 1, 0, 0 );
		glRotatef( -sys->roll, 0, 0, 1 );
		glRotatef( 180-sys->angle, 0, 1, 0 );
	}  else if( sys->viewStyle == 2 ){
		glRotatef( sys->pitch, 1, 0, 0 );
		glRotatef( sys->roll, 0, 0, 1 );
		glRotatef( 90-sys->angle, 0, 1, 0 );
	} else if( sys->viewStyle == 3 ){
		glRotatef( sys->deltaRot/10, 0, 1, 0 );
		glRotatef( sys->deltaPitch/10, 1, 0, 0 );
		glRotatef( -sys->pitch, 1, 0, 0 );
		glRotatef( -sys->roll, 0, 0, 1 );
		glRotatef( 270-sys->angle, 0, 1, 0 );
	}
	for( i = 0; i < backdrops; i++ ){

		float xSize = backdropWidth[i]/4.0f;
		float ySize = backdropHeight[i]/4.0f;
		float back = sqrt( 250000-xSize*xSize-ySize*ySize )-50;
		glBindTexture( GL_TEXTURE_2D, backdrop[i] );
		glBegin( GL_QUADS );
			glTexCoord2d( 0, 0 );
			glVertex3f( -xSize, -ySize, -back );
			glTexCoord2d( 1, 0 );
			glVertex3f( xSize, -ySize, -back );
			glTexCoord2d( 1, 1 );
			glVertex3f( xSize, ySize, -back );
			glTexCoord2d( 0, 1 );
			glVertex3f( -xSize, ySize, -back );
		glEnd();
	}
	glEnable( GL_DEPTH_TEST );
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Solarsystem: %@>", self->ID];
}

- (NSDictionary *) flags{
	return flags;
}

- (NSMutableArray *) planets{
	return planets;
}

- (NSMutableArray *) links{
	return links;
}


- (void) dealloc{
	[types release];
	[super dealloc];
}

@end
