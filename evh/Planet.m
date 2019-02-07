//
//  Planet.m
//  Event Horizon
//
//  Created by user on Thu Aug 08 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

// WHAT IF SHIPYARD, BUT NO SHIPS MEET REQUIREMENTS?

#import "Planet.h"
#import "Controller.h"
#import "Cargo.h"
#import "Weapon.h"
#import "Mod.h"
#import "Mission.h"
#import "main.h"
#import "Stack.h"
#import "EHButton.h"
#import "aglString.h"
#import "Texture.h"
#import "ControllerThreading.h"

extern BOOL debug;

GLuint myTex = 0, missionEndTex = 0;
int mX, mY;						// used for selection buffer
BOOL dirty = YES;				// redraw texture
BOOL dirtyShips = YES;			// reget ships
BOOL dirtyMods = YES;			// reget mods
BOOL dirtyMissions = YES;		// reget missions
BOOL doneDirty = YES;			// texture dirty for done missions
int goodIndex = 0;
int shipIndex = 0;
int modIndex = 0;
int missionIndex = 0;
NSMutableArray *ships;			// ships in shipyard
NSMutableArray *mods;			// mods available
NSMutableArray *missions;		// missions
NSMutableArray *doneMissions;	// missions completed by landing on this planet
GLuint planetFont = 0;
GLuint planetList;
#ifdef __ppc__
float planetLightPos[3] = { 200, 50, 1000 };
#else
float planetLightPos[3] = { 1000, 0, 0 };
#endif
BOOL planetShipInfo;
NSMutableDictionary *planetSounds;
NSString *soundNames[5] = { @"Trade", @"Mod", @"Ship", @"Mission", @"Refuel" };

int canSellGoods();
int canBuyGoods();
int canBuyShip();
void getShips();
void getMods();
int canBuyMod();
int canSellMod();
void getMissions();
int canGetMission();
void missionEndTexture();
void missionEvent( EventRef event );
void nsMissionEvent( NSEvent *event );

void planetLeave();
BOOL planetCanRefuel();
void planetRefuel();
BOOL planetCanMission();
void planetMission();
BOOL planetCanTrade();
void planetTrade();
BOOL planetCanShip();
void planetShip();
BOOL planetCanMod();
void planetMod();
void planetAcceptMission();
BOOL planetCanBuy();
void planetBuyGoods();
void planetBuyShip();
void planetBuyMod();
BOOL planetCanSell();
void planetSellGoods();
void planetSellMod();
void planetMissionDone();
void planetToggleShip();

enum {
	STANDARD,
	TRADING,
	SHIPYARD,
	MOD,
	MISSION
} mode = STANDARD;

@implementation Planet

+ (void) initialize{
	int i;
	NSString *oldPath;

	[super initialize];
	
	planetList = glGenLists( 1 );
	glNewList( planetList, GL_COMPILE );
	gluSphere( sphereObj, 1, 20, 20 );	
	glEndList();
	
	// set up sounds
	oldPath = [[NSFileManager defaultManager] currentDirectoryPath];
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];
	planetSounds = [[NSMutableDictionary alloc] init];
	for( i = 0; i < 5; i++ ){
		NSURL *url = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@.mp3", soundNames[i]]];
		NSMovie *movie = [[[NSMovie alloc] initWithURL:url byReference:TRUE] autorelease];
		NSMovieView *view = [[[NSMovieView alloc] init] autorelease];
		[view setMovie:movie];
		[view setVolume:5];
		[planetSounds setObject:view forKey:soundNames[i]];
	}
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:oldPath];
}

+ (void) registerFromDictionary:(NSDictionary *)dictionary{
	NSMutableDictionary *plugin = [Controller ensurePlugin:[dictionary objectForKey:@"PluginName"] path:@"planets"];
	Planet *planet = [[Planet alloc] init];
	NSString *pictureURL = [dictionary objectForKey:@"PicturePath"];
	NSString *modelPath = [dictionary objectForKey:@"Model"];

	planet->x = [[[dictionary objectForKey:@"Coords"] objectAtIndex:0] floatValue];
	planet->z = [[[dictionary objectForKey:@"Coords"] objectAtIndex:1] floatValue];
	planet->name = [[dictionary objectForKey:@"Name"] copy];
	planet->ID = [[NSString alloc] initWithFormat:@"%@.%@.%@", [dictionary objectForKey:@"PluginName"], @"planets", [dictionary objectForKey:@"ID"]];
	planet->size = [[dictionary objectForKey:@"Size"] floatValue];
	planet->deltaRot = [[dictionary objectForKey:@"Speed"] floatValue];
	planet->mass = [[dictionary objectForKey:@"Mass"] intValue];
	planet->initData = [[NSMutableArray alloc] initWithObjects:[dictionary objectForKey:@"Orbit"],nil];
	planet->description = [[dictionary objectForKey:@"Description"] copy];
	planet->goods = [[dictionary objectForKey:@"Goods"] mutableCopyWithZone:nil];
	if( pictureURL ){
		planet->texFile = [[[[NSFileManager defaultManager] currentDirectoryPath] stringByAppendingFormat:@"/%@",pictureURL] retain];
		planet->texNum = 1;
		planet->texture = calloc( 1, sizeof( GLuint ) );
	} else if( modelPath ){
		planet->model = [[Object3D alloc] init];
		[planet->model loadOBJ:modelPath];
	}
	planet->flags = [[dictionary objectForKey:@"Flags"] copy];
	planet->shipFlags = [[dictionary objectForKey:@"ShipFlags"] copy];
	if( planet->shipFlags )
		planet->shipyard = YES;
	planet->modFlags = [[dictionary objectForKey:@"ModFlags"] copy];
	if( planet->modFlags )
		planet->mod = YES;
	planet->missionFlags = [[dictionary objectForKey:@"MissionFlags"] copy];
	if( planet->missionFlags )
		planet->mission = YES;
	planet->pitch = [dictionary objectForKey:@"Tilt"] ? [[dictionary objectForKey:@"Tilt"] floatValue] : 0;
	planet->inclination = [dictionary objectForKey:@"Inclination"] ? [[dictionary objectForKey:@"Inclination"] floatValue]*pi/180 : 0;
	if( [dictionary objectForKey:@"Ring"] ){
		planet->ringTex = [[[[NSFileManager defaultManager] currentDirectoryPath] stringByAppendingFormat:@"/%@",[[dictionary objectForKey:@"Ring"] objectForKey:@"Path"]] retain];
		planet->ringSize = [[[dictionary objectForKey:@"Ring"] objectForKey:@"Size"] floatValue];
	}
	if( [dictionary objectForKey:@"Atmosphere"] ){
		NSDictionary *atmos = [dictionary objectForKey:@"Atmosphere"];
		//NSData *color = [atmos objectForKey:@"Ambient"];
		int i;
		planet->atmosTex = [[[[NSFileManager defaultManager] currentDirectoryPath] stringByAppendingFormat:@"/%@",[atmos objectForKey:@"Path"]] retain];
		planet->atmosSize = [[atmos objectForKey:@"Size"] floatValue] / planet->size;
		planet->atmosSpeed = [[atmos objectForKey:@"Speed"] floatValue];
		//for( i = 0; i < 3; i++ )
		//	planet->atmosAmbient[i] = ((unsigned char*)[color bytes])[0]/255.;
		//planet->atmosAmbient[3] = 1.0f;
	}
	if( [dictionary objectForKey:@"Replace"] ){
		NSArray *temp = [[dictionary objectForKey:@"Replace"] componentsSeparatedByString:@"."];
		NSMutableDictionary *d = [Controller ensurePlugin:[temp objectAtIndex:0] path:@"planets"];
		[d setObject:planet forKey:[temp objectAtIndex:2]];
		planet->ID = [dictionary objectForKey:@"Replace"];
	} else
		[plugin setObject:planet forKey:[dictionary objectForKey:@"ID"]];
}

- init{
	self = [super init];
	free( texture );
	texNum = 3;
	texture = calloc( texNum, sizeof( GLuint ) );
	return self;
}

- (void) finalize{
	id temp = [initData objectAtIndex:0];
	if( !temp )
		return;
	if( [temp isKindOfClass:[NSString class]] )
		[self setOrbit:[Controller componentNamed:temp]];
	else if( [temp isKindOfClass:[NSNumber class]] ){
		SPACEOBJECT_ORIGIN->mass = [[initData objectAtIndex:0] intValue];
		[self setOrbit:SPACEOBJECT_ORIGIN];
	}
	[initData release];
}

- (NSString *) description{
	return [NSString stringWithFormat:@"<Planet: %@>", self->ID];
}

- (void) orbit{
	theta += w*sys->FACTOR;
	if( theta > 360 )
		theta = 0;
	else if( theta < -360 )
		theta = 0;
	x = centerOfRotation->x + cos( theta*pi/180 )*distance;
	z = centerOfRotation->z + sin( theta*pi/180 )*distance*cos(inclination);
	y = centerOfRotation->y + sin( theta*pi/180 )*distance*sin(inclination);
}

- (void) update{
	if( centerOfRotation )
		[self orbit];
	
	angle += deltaRot*sys->FACTOR;
	atmosRot += atmosSpeed*sys->FACTOR;
}

- (void) drawRing{
	if( ringSize == 0 )
		return;

	[self position];
	glColor3f( 1, 1, 1 );
	glRotatef( -90, 1, 0, 0 );
	glRotatef( pitch, 1, 0, 0 );
	glScalef( ringSize, ringSize, ringSize );
	glBindTexture( GL_TEXTURE_2D, texture[1] );
	glBegin( GL_QUADS );
		glTexCoord2i( 0, 0 );
		glVertex2i( -1, -1 );
		glTexCoord2i( 0, 1 );
		glVertex2i( -1, 1 );
		glTexCoord2i( 1, 1 );
		glVertex2i( 1, 1 );
		glTexCoord2i( 1, 0 );
		glVertex2i( 1, -1 );
	glEnd();
}

- (void) setUpTexture{
	Rect rect = {0, 0, 256, 256};
	GWorldPtr offScreen;
	CGrafPtr port;
	GDHandle gdh;
	RGBColor color;
	QDErr err;
		
	if( err = NewGWorld( &offScreen, 0, &rect, NULL, NULL, 0 ) ){
		NSLog( @"Error on NewGWorld: %d", err );
		return;
	}
	
	/* This is a full planet description text:
	This is Mars, the main planet in this system.  This planet is red because of the high iron content of its composition.  How many words can fit in this description?  Surely not too many.  Let's just see how far we can go before it runs out on the screen.  It seems that we really need to add a whole lot of text to get this to run over. I wonder what will happen when it does run over.  Will it just disappear into the deep, black of space, or will there be some sort of error?  What will happen?  Who can tell?  All I can do is wait and seeÉ  Well, one thing is for sure: it certainly takes a lot of text to run out of room.  Do you think that this will be enough to satisfy the desires of every Event Horizon developer?  ÁI certainly hope so!
	Chars: 743
	Words: 150
	*/
	
	GetGWorld( &port, &gdh );
	SetGWorld( offScreen, NULL );
	
	color.red = color.green = color.blue = 0;
	RGBForeColor( &color );
	PaintRect( &rect );
	
	if( mode == STANDARD ){
		rect.left = 1;
		color.green = 0xFFFF;
		RGBForeColor( &color );
		TXNDrawCFStringTextBox( (CFStringRef)description, &rect, NULL, NULL );
	} else if( mode == SHIPYARD ){
		Spaceship *ship = [ships objectAtIndex:shipIndex];
		rect.left = 1;
		color.green = 0xFFFF;
		RGBForeColor( &color );
		if( !planetShipInfo ){
			rect.bottom = 255;
			TXNDrawCFStringTextBox( (CFStringRef)ship->description, &rect, NULL, NULL );
		} else {
			id key;
			NSEnumerator *keys;
			rect.top = 1;
			rect.right = 128;
			rect.bottom = 17;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Max Speed: %d", (int)(ship->MAX_VELOCITY*50)], &rect, NULL, NULL );
			rect.left = 128;
			rect.right = 255;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Acceleration: %d", (int)(ship->thrust/ship->mass*50)], &rect, NULL, NULL );
			rect.left = 1;
			rect.right = 128;
			rect.top = rect.bottom-4;
			rect.bottom += 16;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Max Rotation: %d", (int)(ship->MAX_ANGULAR_VELOCITY)], &rect, NULL, NULL );
			rect.left = 128;
			rect.right = 255;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Rotational Accel: %d", (int)(ship->ANGULAR_ACCELERATION)], &rect, NULL, NULL );
			rect.left = 1;
			rect.right = 128;
			rect.top = rect.bottom-4;
			rect.bottom += 16;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Size: %dm", (int)(ship->size*100)], &rect, NULL, NULL );
			rect.left = 128;
			rect.right = 255;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Mass: %d", ship->mass], &rect, NULL, NULL );
			rect.left = 1;
			rect.right = 128;
			rect.top = rect.bottom-4;
			rect.bottom += 16;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Mod Space: %d", ship->modSpace], &rect, NULL, NULL );
			rect.left = 128;
			rect.right = 255;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Cargo Space: %d", ship->cargoSpace], &rect, NULL, NULL );
			rect.left = 1;
			rect.right = 128;
			rect.top = rect.bottom-4;
			rect.bottom += 16;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Shields: %d", ship->maxShield], &rect, NULL, NULL );
			rect.left = 128;
			rect.right = 255;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Armor: %d", ship->maxArmor], &rect, NULL, NULL );
			// rect.bottom = 65
			
			rect.top = 72;
			rect.bottom = rect.top + 20;
			rect.left = 1;
			rect.right = 128;
			color.red = 0xFFFF;
			RGBForeColor( &color );
			TXNDrawCFStringTextBox( (CFStringRef)@"Default Loading", &rect, NULL, NULL );
			color.red = 0;
			rect.bottom += 4;
			RGBForeColor( &color );
			keys = [ship->initData keyEnumerator];
			while( key = [keys nextObject] ){
				int num = [[ship->initData objectForKey:key] intValue];
				rect.top = rect.bottom-4;
				rect.bottom += 16;
				rect.right = 255;
				TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"%@: %dx", [[Controller componentNamed:key] name], num], &rect, NULL, NULL );
			}
		}
		rect.left = 1;
		rect.right = 128;
		rect.top = 240;
		rect.bottom = 255;
		color.red = 0xFFFF;
		RGBForeColor( &color );
		TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Price: %d", ship->price], &rect, NULL, NULL );
		rect.left = 128;
		rect.right = 255;
		TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"You have: %d", sys->money], &rect, NULL, NULL );
	} else if( mode == MOD ){
		rect.left = 1;
		rect.bottom = 100;
		color.green = 0xFFFF;
		RGBForeColor( &color );
		TXNDrawCFStringTextBox( (CFStringRef)[[mods objectAtIndex:modIndex] desc], &rect, NULL, NULL );
		rect.top = 210;
		rect.bottom = 225;
		rect.right = 128;
		color.red = 0xFFFF;
		RGBForeColor( &color );
		if( [[mods objectAtIndex:modIndex] max] != -1 ){
			id mod = [sys hasMod:[mods objectAtIndex:modIndex]];
			if( mod && [[mods objectAtIndex:modIndex] max] == [mod count] ){
				color.green = 0;
				RGBForeColor( &color );
				TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Max: %d", [[mods objectAtIndex:modIndex] max]], &rect, NULL, NULL );
				color.green = 0xFFFF;
				RGBForeColor( &color );
			} else
				TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Max: %d", [[mods objectAtIndex:modIndex] max]], &rect, NULL, NULL );
		} else
			TXNDrawCFStringTextBox( (CFStringRef)@"Max: N/A", &rect, NULL, NULL );
		rect.left = 128;
		rect.right = 255;
		TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Current: %d", [[sys hasMod:[mods objectAtIndex:modIndex]] count]], &rect, NULL, NULL );
		rect.left = 1;
		rect.right = 128;
		rect.top = 225;
		rect.bottom = 240;
		if( [[mods objectAtIndex:modIndex] mass] > sys->modSpace ){
			color.green = 0;
			RGBForeColor( &color );
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Mass: %d", [[mods objectAtIndex:modIndex] mass]], &rect, NULL, NULL );
			color.green = 0xFFFF;
			RGBForeColor( &color );
		} else
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Mass: %d", [[mods objectAtIndex:modIndex] mass]], &rect, NULL, NULL );
		rect.left = 128;
		rect.right = 255;
		TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Space: %d", sys->modSpace], &rect, NULL, NULL );
		rect.top = 240;
		rect.bottom = 255;
		rect.left = 1;
		rect.right = 128;
		if( [[mods objectAtIndex:modIndex] price] > sys->money ){
			color.green = 0;
			RGBForeColor( &color );
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Price: %d", [[mods objectAtIndex:modIndex] price]], &rect, NULL, NULL );
			color.green = 0xFFFF;
			RGBForeColor( &color );
		} else
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Price: %d", [[mods objectAtIndex:modIndex] price]], &rect, NULL, NULL );
		rect.left = 128;
		rect.right = 255;
		TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"You have: %d", sys->money], &rect, NULL, NULL );
	} else if( mode == TRADING ){
		int i;
		color.blue = 0xFFFF;
		RGBForeColor( &color );
		rect.top = goodIndex*20+1;
		rect.bottom = rect.top + 14;
		PaintRect( &rect );
		color.blue = 0;
		color.green = 0xFFFF;
		RGBForeColor( &color );
		for( i = 0; i < [goods count]; i++ ){
			NSDictionary *good = [goods objectAtIndex:i];
			int count = 0, index;
			
			for( index = 0; index < [sys->cargo count]; index++ ){
				if( [((Cargo*)[sys->cargo objectAtIndex:index])->name isEqualToString:[good objectForKey:@"Name"]] )
					count += ((Cargo*)[sys->cargo objectAtIndex:index])->size;
			}
			rect.left = 1;
			rect.right = 128;
			rect.top = i*20;
			rect.bottom = rect.top + 20;
			TXNDrawCFStringTextBox( (CFStringRef)[good objectForKey:@"Name"],  &rect, NULL, NULL );
			rect.left = 128;
			rect.right = 192;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"%d", count], &rect, NULL, NULL );
			rect.left = 192;
			rect.right = 255;
			TXNDrawCFStringTextBox( (CFStringRef)[(NSNumber*)[good objectForKey:@"Price"] stringValue], &rect, NULL, NULL );
		}
	} else if( mode == MISSION ){
		if( [missions count] > 0 ){
			rect.left = 1;
			rect.bottom = 240;
			color.green = 0xFFFF;
			RGBForeColor( &color );
			TXNDrawCFStringTextBox( (CFStringRef)((Mission*)[missions objectAtIndex:missionIndex])->description, &rect, NULL, NULL );
			color.red = 0xFFFF;
			RGBForeColor( &color );
			rect.top = 240;
			rect.right = 128;
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Payment: %d", ((Mission*)[missions objectAtIndex:missionIndex])->payment], &rect, NULL, NULL );
			rect.left = 192;
			rect.right = 255;
			if( ((Mission*)[missions objectAtIndex:missionIndex])->size > sys->cargoSpace ){
				color.green = 0;
				RGBForeColor( &color );
			}
			TXNDrawCFStringTextBox( (CFStringRef)[NSString stringWithFormat:@"Weight: %d", ((Mission*)[missions objectAtIndex:missionIndex])->size], &rect, NULL, NULL );
		}
	}
	SetGWorld( port, gdh );
	[Texture loadTextureFromGWorld:offScreen into:&myTex];
}

- (void) draw{
	if( ![self visible] )
		return;
	[self position];
	glColor3f( 1, 1, 1 );	// don't use previous color
	if( !model )
		glRotatef( -90, 1, 0, 0 );
	glRotatef( pitch, 1, 0, 0 );
	glScalef( size, size, size );
	if( model ){		// draw spacestation
		glBindTexture( GL_TEXTURE_2D, model->texture[0] );
		[model drawObject];
	} else {			// draw planet
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glCallList( planetList );
		if( atmosSize != 0 ){
			glBindTexture( GL_TEXTURE_2D, texture[2] );
			glRotatef( atmosRot, 0, 0, -1 );
			glScalef( atmosSize, atmosSize, atmosSize );
			glCallList( planetList );
		}
	}
}

- (NSMutableArray *) goods{
	return goods;
}

- (NSString *) shipFlags{
	return shipFlags;
}

- (void) setShipFlags:(NSString *)flags{
	[shipFlags release];
	shipFlags = [flags retain];
}

- (NSString *) modFlags{
	return modFlags;
}

- (void) setModFlags:(NSString *)flags{
	[modFlags release];
	modFlags = [flags retain];
}

- (NSString *) missionFlags{
	return missionFlags;
}

- (void) setMissionFlags:(NSString *)flags{
	[missionFlags release];
	missionFlags = [flags retain];
}

- (void) setTextureFile:(NSString *)file{
	[texFile release];
	texFile = [file retain];
}

- (void) setModel:(Object3D *)model{
	[self->model release];
	self->model = [model retain];
}

- (float) inclination{
	return inclination;
}

- (void) setInclination:(float)f{
	inclination = f;
}



@end

void setUpPlanet(){
	int i;
	
	mX = -1;
	mY = -1;
	[Stack pushWithScene:@"Planet"];
	[pauseLock lock];
	if( doneMissions == nil )
		doneMissions = [[NSMutableArray alloc] initWithCapacity:4];
	for( i = 0; i < [sys->missions count]; i++ ){
		Mission *mission = [sys->missions objectAtIndex:i];
		if( mission->endPlanet == sys->curPlanet ){
			[doneMissions addObject:mission];
			[sys->missions removeObjectAtIndex:i--];
		}
	}
	sys->secondaryIndex = -1;
	
	[EHButton clear];
	[EHButton newButton:@"Leave" atX:sys->screenWidth-164 y:50 width:128 callback:planetLeave withParam:NO active:YES isActive:nil];
	[EHButton newButton:@"Refuel" atX:sys->screenWidth-164 y:260 width:128 callback:planetRefuel withParam:NO active:NO isActive:planetCanRefuel];
	[EHButton newButton:@"Missions" atX:sys->screenWidth-164 y:220 width:128 callback:planetMission withParam:NO active:NO isActive:planetCanMission];
	[EHButton newButton:@"Trade" atX:sys->screenWidth-164 y:100 width:128 callback:planetTrade withParam:NO active:NO isActive:planetCanTrade];
	[EHButton newButton:@"Buy Ship" atX:sys->screenWidth-164 y:140 width:128 callback:planetShip withParam:NO active:NO isActive:planetCanShip];
	[EHButton newButton:@"Ship Mods" atX:sys->screenWidth-164 y:180 width:128 callback:planetMod withParam:NO active:NO isActive:planetCanMod];
	[EHButton storeSet:@"Planet"];
	
	planetShipInfo = NO;
	
	[pauseLock lock];
}

void drawPlanet(){	
	if( planetFont == 0 ){
		short fNum = FMGetFontFamilyFromName( "\pTimes" );
		if( fNum == kInvalidFontFamily )
			fNum = 0;
		//planetFont = BuildFontGL( gaglContext, fNum, normal, 12 );
	}

	if( dirty ){
		[sys->curPlanet setUpTexture];
		dirty = NO;
	}
	
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightfv( GL_LIGHT1, GL_POSITION, planetLightPos );
	
#ifdef __ppc__	
	glEnable( GL_LIGHTING );
#endif
	glColor3f( 1, 1, 1 );
	glLoadIdentity();
	if( sys->screenWidth > 800 ){
		glTranslatef( 200, sys->screenHeight-200, -200 );
		glScalef( 175, 175, 25 );
		if( !sys->curPlanet->model ){
			glRotatef( -90, 1, 0, 0 );
			glRotatef( sys->curPlanet->angle, 0, 0, 1 );
		} else {
			glRotatef( 35, 1, 0, 1 );
			glRotatef( sys->curPlanet->angle, 0, 1, 0 );
		}
		if( !sys->curPlanet->model ){
			glBindTexture( GL_TEXTURE_2D, sys->curPlanet->texture[0] );
			glCallList( planetList );
		} else {
			glBindTexture( GL_TEXTURE_2D, sys->curPlanet->model->texture[0] );
			[sys->curPlanet->model drawObject];
		}
		sys->curPlanet->angle += sys->curPlanet->deltaRot*sys->FACTOR;

		glLoadIdentity();
		glTranslatef( 575, sys->screenHeight-200, 0 );
	} else {
		glTranslatef( 200, sys->screenHeight-200, 0 );
	}
	glDisable( GL_LIGHTING );
	glBindTexture( GL_TEXTURE_2D, myTex );
	glBegin( GL_QUADS );
		glTexCoord2f( 0, 1 );
		glVertex3f( -175, -175, -1 );
		glTexCoord2f( 1, 1 );
		glVertex3f( 175, -175, -1 );
		glTexCoord2f( 1, 0 );
		glVertex3f( 175, 175, -1 );
		glTexCoord2f( 0, 0 );
		glVertex3f( -175, 175, -1 );
	glEnd();

	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, sys->texture[WIDGET_TEXTURE] );

	if( mode == TRADING ){
		if( (sys->screenWidth > 800 && mX > 400 && mX < 750) || (sys->screenWidth <= 800 && mX > 25 && mX < 575) ){
			float temp = mY;
			float factor = 350./256.*20.;
			temp -= 25;
			temp /= factor;
			if( temp < [sys->curPlanet->goods count] ){
				goodIndex = temp;
				dirty = YES;
			}
		}
	} else if( mode == SHIPYARD ){
		int i, count = (sys->screenWidth-361)/100;
		static int offset = 0;

		if( shipIndex - offset > count )	// off to the right
			offset = shipIndex - count;
		else if( shipIndex < offset )	// to the left
			offset = shipIndex;
	
		if( offset == 0 )
			glColor3f( .5, .5, .5 );
		else {
			glColor3f( 1, 1, 1 );
			if( mX >= 16 && mX <= 48 && mY <= sys->screenHeight-118 && mY >= sys->screenHeight-182 )
				offset--;
			if( shipIndex > offset + count ){
				shipIndex = offset + count;
				dirty = YES;
			}
		}
		glLoadIdentity();
		glBegin( GL_QUADS );
			glTexCoord2f( 0, .375 );
			glVertex2f( 16, 118 );
			glTexCoord2f( 0, .25 );
			glVertex2f( 48, 118 );
			glTexCoord2f( .25, .25 );
			glVertex2f( 48, 182 );
			glTexCoord2f( .25, .375 );
			glVertex2f( 16, 182 );
		glEnd();
		
		if( [ships count] - offset > count+1 ){
			glColor3f( 1, 1, 1 );
			if( mX >= 149+100*count && mX <= 191+100*count && mY <= sys->screenHeight-118 && mY >= sys->screenHeight-182 )
				offset++;
			if( shipIndex < offset ){
				shipIndex = offset;
				dirty = YES;
			}
		} else
			glColor3f( .5, .5, .5 );
		glBegin( GL_QUADS );
			glTexCoord2f( 0, .25 );
			glVertex2f( 150+100*count, 118 );
			glTexCoord2f( 0, .375 );
			glVertex2f( 192+100*count, 118 );
			glTexCoord2f( .25, .375 );
			glVertex2f( 192+100*count, 182 );
			glTexCoord2f( .25, .25 );
			glVertex2f( 150+100*count, 182 );
		glEnd();

		for( i = offset; i < [ships count] && (i-offset) <= count; i++ ){
			Spaceship *ship = [ships objectAtIndex:i];
			glLoadIdentity();
			if( shipIndex == i ){	// highlight background
				glColor3f( 0, 0, 1 );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glBegin( GL_QUADS );
					glVertex3f( 50+100*(i-offset), 100, -200 );
					glVertex3f( 150+100*(i-offset), 100, -200 );
					glVertex3f( 150+100*(i-offset), 200, -200 );
					glVertex3f( 50+100*(i-offset), 200, -200 );
				glEnd();
				glColor3f( 0, 0, 0 );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glBegin( GL_QUADS );
					glVertex3f( 51+100*(i-offset), 101, -150 );
					glVertex3f( 149+100*(i-offset), 101, -150 );
					glVertex3f( 149+100*(i-offset), 199, -150 );
					glVertex3f( 51+100*(i-offset), 199, -150 );
				glEnd();
			}
			glColor3f( 1, 1, 1 );
			glTranslatef( 100+100*(i-offset), 150, -100 );
			glRotatef( 90, 0, 1, 0 );
			glRotatef( 15, 0, 0, 1 );
			glRotatef( 15*sys->t, 0, 1, 0 );
			glScalef( 40, 40, 40 );
			glBindTexture( GL_TEXTURE_2D, ship->texture[0] );
#ifdef __ppc__
			glEnable( GL_LIGHTING );
#endif
			[ship drawObject];
			glDisable( GL_LIGHTING );
			glLoadIdentity();
			glColor3f( 0, 1, 0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glRasterPos3f( 75+100*(i-offset), 88, -200 );
			DrawCStringGL( [ship->name cString], planetFont );
			if( mX <= 150+100*(i-offset) && mX >= 50+100*(i-offset) && mY <= sys->screenHeight-100 && mY >= sys->screenHeight-200 ){
				shipIndex = i;
				dirty = YES;
			}
		}
	} else if( mode == MOD ){
		int i, count = (sys->screenWidth-361)/100;
		static int offset = 0;
		
		if( modIndex - offset > count )	// off to the right
			offset = modIndex - count;
		else if( modIndex < offset )	// to the left
			offset = modIndex;
	
		if( offset == 0 )
			glColor3f( .5, .5, .5 );
		else {
			glColor3f( 1, 1, 1 );
			if( mX >= 16 && mX <= 48 && mY <= sys->screenHeight-118 && mY >= sys->screenHeight-182 )
				offset--;
			if( modIndex > offset + count ){
				modIndex = offset + count;
				dirty = YES;
			}
		}
		glLoadIdentity();
		glBegin( GL_QUADS );
			glTexCoord2f( 0, .375 );
			glVertex2f( 16, 118 );
			glTexCoord2f( 0, .25 );
			glVertex2f( 48, 118 );
			glTexCoord2f( .25, .25 );
			glVertex2f( 48, 182 );
			glTexCoord2f( .25, .375 );
			glVertex2f( 16, 182 );
		glEnd();
		
		if( [mods count] - offset > count+1 ){
			glColor3f( 1, 1, 1 );
			if( mX >= 149+100*count && mX <= 191+100*count && mY >= sys->screenHeight-182 )
				offset++;
			if( modIndex < offset ){
				modIndex = offset;
				dirty = YES;
			}
		} else
			glColor3f( .5, .5, .5 );
		glBegin( GL_QUADS );
			glTexCoord2f( 0, .25 );
			glVertex2f( 149+100*count, 118 );
			glTexCoord2f( 0, .375 );
			glVertex2f( 191+100*count, 118 );
			glTexCoord2f( .25, .375 );
			glVertex2f( 191+100*count, 182 );
			glTexCoord2f( .25, .25 );
			glVertex2f( 149+100*count, 182 );
		glEnd();

		for( i = offset; i < [mods count] && (i-offset) <= count; i++ ){
			GLuint tex;
			const char *string;

			tex = [[mods objectAtIndex:i] texture];
			string = [[[mods objectAtIndex:i] name] cString];
			glLoadIdentity();
			if( modIndex == i ){
				glColor3f( 0, 0, 1 );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glBegin( GL_QUADS );
					glVertex3f( 67+100*(i-offset), 117, -200 );
					glVertex3f( 133+100*(i-offset), 117, -200 );
					glVertex3f( 133+100*(i-offset), 183, -200 );
					glVertex3f( 67+100*(i-offset), 183, -200 );
				glEnd();
			}
			glColor3f( 1, 1, 1 );
			glTranslatef( 100+100*(i-offset), 150, 0 );
			glBindTexture( GL_TEXTURE_2D, tex );
			glBegin( GL_QUADS );
				glTexCoord2f( 0, 0 );
				glVertex2f( -32, -32 );
				glTexCoord2f( 1, 0 );
				glVertex2f( 32, -32 );
				glTexCoord2f( 1, 1 );
				glVertex2f( 32, 32 );
				glTexCoord2f( 0, 1 );
				glVertex2f( -32, 32 );
			glEnd();
			glLoadIdentity();
			glColor3f( 0, 1, 0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glRasterPos3f( 60+100*(i-offset), 88, -200 );
			DrawCStringGL( string, planetFont );
			if( mX <= 150+100*(i-offset) && mX >= 50+100*(i-offset) && mY <= sys->screenHeight-100 && mY >= sys->screenHeight-200 ){
				modIndex = i;
				dirty = YES;
			}
			if( [sys hasMod:[mods objectAtIndex:i]] ){
				id mod = [sys hasMod:[mods objectAtIndex:i]];
				glRasterPos3f( 92+100*(i-offset), 190, -200 );
				DrawCStringGL( [[NSString stringWithFormat:@"x%d", [mod count]] cString], planetFont );
			}
		}
	} else if( mode == MISSION ){
		int i;
		int x, y;
		const char *name;
		
		if( sys->screenWidth <= 800 ){
			x = 10;
			y = 200;
		} else {
			x = 100;
			y = 300;
		}
		glBindTexture( GL_TEXTURE_2D, 0 );
		glColor3f( 0, 1, 0 );
		for( i = 0; i < [missions count]; i++ ){
			name = [((Mission*)[missions objectAtIndex:i])->title cString];
			glLoadIdentity();
			if( i == missionIndex ){
				glColor3f( 0, 0, 1 );
				glRasterPos2f( x-20, y-15*i );
				DrawCStringGL( ">>", font );				
				glColor3f( 0, 1, 0 );
			}
			glRasterPos2f( x, y-15*i );
			DrawCStringGL( name, font );
			if( mX >= x-20 && mX <= x+230 && mY <= sys->screenHeight-(y-15*i-3) && mY >= sys->screenHeight-(y-15*i+12) ){
				missionIndex = i;
				dirty = YES;
			}
		}
	}

	glColor3f( 0, 1, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glLoadIdentity();
	glRasterPos2f( 50, 25 );
	DrawCStringGL( [[NSString stringWithFormat:@"Money: %d    Cargo space: %d    Mod space: %d", sys->money, sys->cargoSpace, sys->modSpace] cString], font );

	mX = -1;
	mY = -1;
	
	if( [doneMissions count] > 0 ){
		if( doneDirty ){
			missionEndTexture();
			[EHButton clear];
			[EHButton newButton:@"OK" atX:sys->screenWidth/2-64 y:sys->screenHeight/2-162 width:128 callback:planetMissionDone withParam:NO active:YES isActive:nil];
		}
		eventScene = missionEvent;
		nsEventScene = nsMissionEvent;
		glLoadIdentity();
		glTranslatef( 0, 0, -.5 );
		glColor3f( 1, 1, 1 );
		glBindTexture( GL_TEXTURE_2D, missionEndTex );
		glBegin( GL_QUADS );
			glTexCoord2f( 0, 1 );
			glVertex2f( sys->screenWidth/2-175, sys->screenHeight/2-175 );
			glTexCoord2f( 1, 1 );
			glVertex2f( sys->screenWidth/2+175, sys->screenHeight/2-175 );
			glTexCoord2f( 1, 0 );
			glVertex2f( sys->screenWidth/2+175, sys->screenHeight/2+175 );
			glTexCoord2f( 0, 0 );
			glVertex2f( sys->screenWidth/2-175, sys->screenHeight/2+175 );
		glEnd();
	}

	[EHButton update];
}

void keyPlanet( int key ){
	switch( key ){
		case '\r':
		case 3:
			planetLeave();
			break;
		case 't':
		case 'T':
			planetTrade();
			break;
		case 's':
		case 'S':
			if( mode == TRADING )
				planetSellGoods();
			else if( mode == MOD && canSellMod() > 0 )
				planetSellMod();
			break;
		case 'y':
		case 'Y':
			planetShip();
			break;
		case 'm':
		case 'M':
			planetMod();
			break;
		case 'b':
		case 'B':
			if( mode == TRADING )
				planetBuyGoods();
			else if( mode == SHIPYARD )
				planetBuyShip();
			else if( mode == MOD )
				planetBuyMod();
			break;
		case 'a':
		case 'A':
			planetAcceptMission();
			break;
		case 30:	// up key
		case NSUpArrowFunctionKey:
			if( mode == TRADING ){
				goodIndex--;
				if( goodIndex == -1 )
					goodIndex = [sys->curPlanet->goods count]-1;
				dirty = YES;			
			} else if( mode == MISSION ){
				missionIndex--;
				if( missionIndex == -1 )
					missionIndex = [missions count]-1;
				dirty = YES;
			}
			break;
		case 31:	// down arrow
		case NSDownArrowFunctionKey:
			if( mode == TRADING ){
				goodIndex++;
				if( goodIndex >= [sys->curPlanet->goods count] )
					goodIndex = 0;
				dirty = YES;
			} else if( mode == MISSION ){
				missionIndex++;
				if( missionIndex >= [missions count] )
					missionIndex = 0;
				dirty = YES;
			}
			break;
		case 28:	// left arrow
		case NSLeftArrowFunctionKey:
			if( mode == SHIPYARD ){
				shipIndex--;
				if( shipIndex == -1 )
					shipIndex = [ships count]-1;
				dirty = YES;
			} else if( mode == MOD ){
				modIndex--;
				if( modIndex == -1 )
					modIndex = [mods count]-1;
				dirty = YES;
			}
			break;
		case 29:	// right arrow
		case NSRightArrowFunctionKey:
			if( mode == SHIPYARD ){
				shipIndex++;
				if( shipIndex >= [ships count] )
					shipIndex = 0;
				dirty = YES;
			} else if( mode == MOD ){
				modIndex++;
				if( modIndex >= [mods count] )
					modIndex = 0;
				dirty = YES;
			}
			break;
		case 'f':
		case 'F':
			planetRefuel();
			break;
		case 'n':
		case 'N':
			planetMission();
			break;
	}
}

int canBuyGoods(){
	if( mode == TRADING && sys->cargoSpace > 0 && sys->money >= [(NSNumber*)[[sys->curPlanet->goods objectAtIndex:goodIndex] objectForKey:@"Price"] intValue] ){
		int i;
		for( i = 0; i < [sys->cargo count]; i++ ){
			if( [[[sys->curPlanet->goods objectAtIndex:goodIndex] objectForKey:@"Name"] isEqualToString:((Cargo*)[sys->cargo objectAtIndex:i])->name] ){
				return i;
			}
		}
		return -1;
	} else
		return -2;
}

int canSellGoods(){
	if( mode == TRADING ){
		int i;
		for( i = 0; i < [sys->cargo count]; i++ ){
			if( [[[sys->curPlanet->goods objectAtIndex:goodIndex] objectForKey:@"Name"] isEqualToString:((Cargo*)[sys->cargo objectAtIndex:i])->name] ){
				return i;
			}
		}
	}
	return -1;
}

int canBuyShip(){
	if( mode != SHIPYARD )
		return 0;
	if( ((Spaceship*)[ships objectAtIndex:shipIndex])->price <= sys->price + sys->money )
		return 1;
	return 0;
}

void getShips(){
	int i;

	if( ships )
		[ships release];
	ships = [[Controller objectsOfType:@"ship" withFlags:sys->curPlanet->shipFlags] retain];
	for( i = 0; i < [ships count]; i++ ){
		Spaceship *temp = [ships objectAtIndex:i];
		if( [[Controller objects:[NSArray arrayWithObject:sys->curPlanet] withFlags:temp->flagRequirements] count] == 0 )
			[ships removeObjectAtIndex:i--];
	}
	dirtyShips = NO;
	shipIndex = 0;
}

void getMods(){
	int i;

	if( mods )
		[mods release];
	mods = [[Controller objectsOfType:@"mod" withFlags:sys->curPlanet->modFlags] retain];
	for( i = 0; i < [mods count]; i++ ){
		if( [[Controller objects:[NSArray arrayWithObject:sys->curPlanet] withFlags:[[mods objectAtIndex:i] flagRequirements]] count] == 0 )
			[mods removeObjectAtIndex:i--];
	}
	dirtyMods = NO;
	modIndex = 0;
}

void getMissions(){
	NSMutableArray *temp = [[NSMutableArray alloc] initWithCapacity:10];
	int i;

	if( missions )
		[missions release];
	missions = [Controller objectsOfType:@"mission" withFlags:sys->curPlanet->missionFlags];
	for( i = 0; i < [missions count]; i++ ){
		Mission *m = [[missions objectAtIndex:i] suitableSystem:sys->system andPlanet:sys->curPlanet];
		if( m )
			[temp addObject:m];
	}
	missions = temp;
	dirtyMissions = NO;
	missionIndex = 0;
}

int canGetMission(){
	if( mode == MISSION && [missions count] > 0 ){
		Mission *mission = [missions objectAtIndex:missionIndex];
		if( mission->size <= sys->cargoSpace )
			return 1;
	}
	return 0;
}

int canBuyMod(){
	id mod = [mods objectAtIndex:modIndex];
	if( mode != MOD )
		return 0;
	if( [mod price] <= sys->money && [mod mass] <= sys->modSpace ){
		if( [mod isKindOfClass:[Weapon class]] ){
			int i;
			for( i = 0; i < [sys->weapons count]; i++ ){
				Weapon *weap = [sys->weapons objectAtIndex:i];
				if( [[weap ID] isEqualToString:[mod ID]] ){
					if( [weap count] < [weap max] || [weap max] == -1 )
						return 1;
					return 0;
				}
			}
			return 1;
		} else if( [mod isKindOfClass:[Mod class]] ){
			int i;
			for( i = 0; i < [sys->mods count]; i++ ){
				Mod *m = [sys->mods objectAtIndex:i];
				if( [[m ID] isEqualToString:[mod ID]] ){
					if( [m count] < [m max] || [m max] == -1 )
						return 1;
					return 0;
				}
			}
			return 1;
		}
	}
	return 0;
}

int canSellMod(){
	id mod = [sys hasMod:[mods objectAtIndex:modIndex]];
	if( mode == MOD && mod ){
		return [mod count];
	}
	return 0;
}

void mousePlanet( int button, int state, int _x, int _y ){
	mX = _x;
	mY = _y;
	if( state == 1 ){
		mX = -1;
		mY = -1;
	}
}

void eventPlanet( EventRef event ){
	UInt32 class, kind;
	
	class = GetEventClass( event );
	kind = GetEventKind( event );
	
	switch( class ){
		case kEventClassKeyboard:
			if( kind == kEventRawKeyDown || kind == kEventRawKeyRepeat){
				if( ![sys functionKey] ){
					char theChar;
					GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &theChar );
					keyPlanet( theChar );
				}
			}
			break;
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				Point p;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				mousePlanet( 0, 0, p.h, p.v );
			} else if( kind == kEventMouseUp ){
				mousePlanet( 0, 1, 0, 0 );
			}
			break;
	}
}

void nsEventPlanet( NSEvent *event ){
	switch( [event type] ){
		case NSLeftMouseDownMask:
			mousePlanet( 0, 0, [event locationInWindow].x, sys->screenHeight-[event locationInWindow].y-22 );
			break;
		case NSLeftMouseUpMask:
			mousePlanet( 0, 1, 0, 0 );
			break;
		case NSKeyDown:
			if( ![sys functionKey] )
				keyPlanet( [[event characters] characterAtIndex:0] );
			break;
		default:
			break;
	}
}

void missionEvent( EventRef event ){
	UInt32 class, kind;
	
	class = GetEventClass( event );
	kind = GetEventKind( event );
	
	if( class == kEventClassKeyboard && (kind == kEventRawKeyDown || kind == kEventRawKeyRepeat) ){
		if( ![sys functionKey] ){
			char theChar;
			GetEventParameter( event, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &theChar );
			if( theChar == '\r' || theChar == 3 ){
				planetMissionDone();
			}
		}
	}
}

void nsMissionEvent( NSEvent *event ){
	if( [event type] == NSKeyDown && ([[event characters] characterAtIndex:0] == '\r' || [[event characters] characterAtIndex:0] == 3) ){
		planetMissionDone();
	}
}

void missionEndTexture(){
	Rect rect = {0, 0, 256, 256};
	GWorldPtr offScreen;
	CGrafPtr port;
	GDHandle gdh;
	RGBColor color;
	QDErr err;

	if( err = NewGWorld( &offScreen, 0, &rect, NULL, NULL, 0 ) ){
		NSLog( @"Error on NewGWorld: %d", err );
		return;
	}

	GetGWorld( &port, &gdh );
	SetGWorld( offScreen, NULL );
	
	color.red = color.green = color.blue = 0;
	RGBForeColor( &color );
	PaintRect( &rect );
	color.red = color.green = color.blue = 0x8888;
	RGBForeColor( &color );
	rect.left = rect.top = 1;
	rect.bottom = rect.right = 255;
	PaintRect( &rect );
	rect.left = rect.top = 3;
	rect.right = rect.bottom = 253;
	color.red = color.green = color.blue = 0x0000;
	RGBForeColor( &color );
	PaintRect( &rect );
	
	color.green = 0xFFFF;
	RGBForeColor( &color );
	rect.left = 10;
	rect.right = 245;
	rect.bottom = 200;
	rect.top = 10;
	TXNDrawCFStringTextBox( (CFStringRef)((Mission*)[doneMissions objectAtIndex:0])->endText, &rect, NULL, NULL );

	SetGWorld( port, gdh );
	[Texture loadTextureFromGWorld:offScreen into:&missionEndTex];
	doneDirty = NO;
}

void planetMissionDone(){
	Mission *mission = [doneMissions objectAtIndex:0];
	[doneMissions removeObjectAtIndex:0];
	doneDirty = YES;
	[mission accomplish];
	eventScene = eventPlanet;
	nsEventScene = nsEventPlanet;
	[EHButton recallSet:@"Planet"];
}

void planetLeave(){
	int i;
	[pauseLock unlock];
	[Stack pop];
	mode = STANDARD;
	dirtyShips = YES;
	dirtyMods = YES;
	dirtyMissions = YES;
	dirty = YES;
	modIndex = 0;				// make safe for next landing
	shipIndex = 0;
	missionIndex = 0;
	goodIndex = 0;
	[sys->date autorelease];
	sys->date = [[sys->date addTimeInterval:86400.0] retain];
	[sys save];
	missionIndex = 0;
	for( i = 0; i < [sys->missions count]; i++ ){
		[[sys->missions objectAtIndex:i] update];
	}
	sys->x = sys->curPlanet->x+sys->curPlanet->size+sys->size;
	sys->z = sys->curPlanet->z+sys->curPlanet->size+sys->size;
	[sys->system setUp];
	[EHButton clear];
	
	[pauseLock unlock];
}

BOOL planetCanRefuel(){
	return sys->fuel < sys->maxFuel && sys->money > 99;
}

void planetRefuel(){
	if( planetCanRefuel() ){
		NSMovieView *sound = [planetSounds objectForKey:@"Refuel"];
		float amount = sys->maxFuel - sys->fuel;
		if( sys->money < amount*100 )
			amount = sys->money/100.;
		sys->fuel += amount;
		sys->money -= amount*100;
		[sound gotoBeginning:nil];
		[sound start:nil];
	}
}

BOOL planetCanMission(){
	return sys->curPlanet->mission && mode != MISSION;
}

void planetMission(){
	if( sys->curPlanet->mission ){
		NSMovieView *sound = [planetSounds objectForKey:@"Mission"];
		if( dirtyMissions )
			getMissions();
		mode = MISSION;
		dirty = YES;
		[EHButton recallSet:@"Planet"];
		[EHButton newButton:@"Accept" atX:sys->screenWidth-528 y:50 width:128 callback:planetAcceptMission withParam:NO active:NO isActive:canGetMission];
		[sound gotoBeginning:nil];
		[sound start:nil];
	}
}

BOOL planetCanTrade(){
	return [sys->curPlanet->goods count] > 0 && mode != TRADING;
}

void planetTrade(){
	if( planetCanTrade() ){
		NSMovieView *sound = [planetSounds objectForKey:@"Trade"];
		mode = TRADING;
		dirty = YES;
		[EHButton recallSet:@"Planet"];
		[EHButton newButton:@"Buy" atX:sys->screenWidth-528 y:50 width:128 callback:planetBuyGoods withParam:NO active:NO isActive:planetCanBuy];
		[EHButton newButton:@"Sell" atX:sys->screenWidth-378 y:50 width:128 callback:planetSellGoods withParam:NO active:NO isActive:planetCanSell];
		[sound gotoBeginning:nil];
		[sound start:nil];
	}
}

BOOL planetCanShip(){
	return sys->curPlanet->shipyard && mode != SHIPYARD;
}

void planetShip(){
	if( planetCanShip() ){
		NSMovieView *sound = [planetSounds objectForKey:@"Ship"];
		if( dirtyShips )
			getShips();
		mode = SHIPYARD;
		dirty = YES;
		[EHButton recallSet:@"Planet"];
		[EHButton newButton:@"Buy" atX:sys->screenWidth-528 y:50 width:128 callback:planetBuyShip withParam:NO active:NO isActive:planetCanBuy];
		[EHButton newButton:@"Info" atX:sys->screenWidth-378 y:50 width:128 callback:planetToggleShip withParam:NO active:YES isActive:nil];
		[sound gotoBeginning:nil];
		[sound start:nil];
	}
}

BOOL planetCanMod(){
	return sys->curPlanet->mod && mode != MOD;
}

void planetMod(){
	if( planetCanMod() ){
		NSMovieView *sound = [planetSounds objectForKey:@"Mod"];
		dirty = YES;
		mode = MOD;
		if( dirtyMods )
			getMods();
		[EHButton recallSet:@"Planet"];
		[EHButton newButton:@"Buy" atX:sys->screenWidth-528 y:50 width:128 callback:planetBuyMod withParam:NO active:NO isActive:planetCanBuy];
		[EHButton newButton:@"Sell" atX:sys->screenWidth-378 y:50 width:128 callback:planetSellMod withParam:NO active:NO isActive:planetCanSell];
		[sound gotoBeginning:nil];
		[sound start:nil];
	}
}

void planetAcceptMission(){
	if( canGetMission() ){
		Mission *mission = [missions objectAtIndex:missionIndex];
		[mission accept];
		[missions removeObjectAtIndex:missionIndex];
		if( missionIndex == [missions count] )
			missionIndex--;
		dirty = YES;
	}
}

BOOL planetCanBuy(){
	return canBuyGoods() >= -1 || canBuyShip() > 0 || canBuyMod() > 0;
}

void planetBuyShip(){
	if( canBuyShip() ){
		sys->money += sys->price;
		while( [sys->weapons count] >= 1 ){
			sys->money += [[sys->weapons lastObject] price];
			[sys->weapons removeLastObject];
		}
		while( [sys->mods count] >= 1 ){
			sys->money += [[sys->mods lastObject] price];
			[sys->mods removeLastObject];
		}
		while( [sys->secondary count] >= 1 ){
			sys->money += [[sys->secondary lastObject] price];
			[sys->secondary removeLastObject];
		}
		[sys setShip:[ships objectAtIndex:shipIndex]];
		sys->money -= sys->price;
		mode = STANDARD;
		dirty = YES;
	}
}

void planetBuyGoods(){
	int i = canBuyGoods();
	Cargo *c;
	if( i < -1 )
		return;
	if( i == -1 ){
		c = [[[Cargo alloc] init] autorelease];
		c->name = [[sys->curPlanet->goods objectAtIndex:goodIndex] objectForKey:@"Name"];
		c->size = 1;
		[sys->cargo addObject:c];
	} else {
		c = [sys->cargo objectAtIndex:i];
		c->size++;
	}	
	dirty = YES;
	sys->cargoSpace--;
	sys->mass++;
	sys->ACCELERATION = sys->thrust/(sys->mass*sys->compensation);
	sys->money -= [(NSNumber*)[[sys->curPlanet->goods objectAtIndex:goodIndex] objectForKey:@"Price"] intValue];
}

void planetBuyMod(){
	if( canBuyMod() ){
		[sys addMod:[mods objectAtIndex:modIndex]];
		dirty = YES;
		sys->money -= [[mods objectAtIndex:modIndex] price];
	}
}

BOOL planetCanSell(){
	return canSellGoods() >= 0|| canSellMod() > 0;
}

void planetSellGoods(){
	Cargo *c;
	int i = canSellGoods();
	if( i < 0 )
		return;
	dirty = YES;
	sys->cargoSpace++;
	c = [sys->cargo objectAtIndex:i];
	if( c->size == 1 ){
		[sys->cargo removeObjectAtIndex:i];
	}
	else
		c->size--;
	sys->money += [(NSNumber*)[[sys->curPlanet->goods objectAtIndex:goodIndex] objectForKey:@"Price"] intValue];
}

void planetSellMod(){
	id sysMod = [sys hasMod:[mods objectAtIndex:modIndex]];
	id mod = [mods objectAtIndex:modIndex];
	sys->money += .75*[mod price];
	sys->modSpace += [mod mass];
	sys->mass -= [mod mass];
	if( [sysMod isKindOfClass:[Weapon class]] ){
		Weapon *weap = (Weapon *)sysMod;
		weap->count--;
		if( weap->count == 0 )
			[sys->weapons removeObject:weap];
	} else if( [sysMod isKindOfClass:[Mod class]] ){
		Mod *mod = (Mod *)sysMod;
		mod->count--;
		if( mod->count == 0 )
			[sys->mods removeObject:mod];
		if( mod->ammo && mod->count == 0 )
			[sys->secondary removeObject:mod];
		if( mod->modType == COMPENSATOR )
			sys->compensation /= mod->value;
	}
	sys->ACCELERATION = sys->thrust/(sys->mass*sys->compensation);
	dirty = YES;
}

void planetToggleShip(){
	planetShipInfo = !planetShipInfo;
	if( !planetShipInfo )
		[EHButton setName:@"Info" forName:@"About"];
	else
		[EHButton setName:@"About" forName:@"Info"];
	dirty = YES;
}