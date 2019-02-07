//
//  Controller.m
//  Event Horizon
//
//  Created by user on Thu Aug 08 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "Controller.h"
#import "ControllerMenu.h"
#import "ControllerGraphics.h"
#import "ControllerScripting.h"
#import "ControllerThreading.h"
#import "KeyMapNames.h"
#import "Map.h"
#import "Prefs.h"
#import "Weapon.h"
#import "Cargo.h"
#import "Background.h"
#import "aglString.h"
#import "Mod.h"
#import "Mission.h"
#import "Inventory.h"
#import "CutScene.h"
#import "SoundManager.h"
#import "Console.h"
#import "StarDust.h"
#import "ShieldSpot.h"
#import "EHMenu.h"
#import "Forklift.h"
#import "Player.h"
#import "Asteroid.h"

KeyMap2 oldKeys;					// used to tell if a keypress is new
extern BOOL mouseMoved;

#define NEW_PRESS( key ) (keys[index[key]] & val[key] && !(oldKeys[index[key]] & val[key]))

BOOL killRot = NO;
BOOL killVertRot = NO;
GLfloat ambient[]= { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat diffuse[] = { 1, 1, 1, 1 };
GLfloat lightPos[] = { 0, 0, 0, 1 };

int explode;

int totalFiles = 0;
int filesDone = 0;

BOOL debug = NO;

@implementation Controller

- initWithName:(NSString*)_name new:(BOOL)new{
//	fprintf( stderr, "Controller init\n" );
	NSString *textureNames[NUM_TEXTURES];
	int i;
	NSArray *temp;
	extern GLuint shieldSpotTexture, menuItemTex;
	
NSLog( @"[sys initWithName]" );
	self = [super init];
	self->ID = @"";
	sys = self;
	name = _name;
	NSLog( @"   init background" );
	bg = [[Background alloc] init];
	ships = [[NSMutableArray alloc] initWithCapacity:10];
	weaps = [[NSMutableArray alloc] initWithCapacity:10];
	asteroids = [[NSMutableArray alloc] initWithCapacity:25];
	missions = [[NSMutableArray alloc] initWithCapacity:5];
	govRecord = [[NSMutableDictionary alloc] initWithCapacity:10];
	frameFront = 0;
	frameRear = 0;
	GRAVITY = 1;
	t = 0;
	hyperTime = 0;
	x = 0;
	z = -12;
	speedx = speedz = deltaRot = 0;
	angle = 0;
	texNum = NUM_TEXTURES;
	texture = calloc( NUM_TEXTURES, sizeof( GLuint ) );
	NSLog( @"   init plugins" );
	[self initPlugins];
	if( new ){
		NSLog( @"   set system" );
		[self setSystem:[Controller componentNamed:@"default.systems.start"]];
		NSLog( @"   set ship" );
		[self setShip:[Controller componentNamed:@"default.ships.start"]];		
	}
	[[Controller componentNamed:@"default.systems"] removeObjectForKey:@"start"];
	[[Controller componentNamed:@"default.ships"] removeObjectForKey:@"start"];
	cargo = [[NSMutableArray alloc] initWithCapacity:cargoSpace];
	NSLog( @"   load textures" );
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];
	textureNames[NAVPANEL_TEXTURE] = @"TabView2.png";
	textureNames[NAVPLANET_TEXTURE] = @"NavPlanet.tiff";
	textureNames[WIDGET_TEXTURE] = @"Widgets.png";
	textureNames[INFOTAB_TEXTURE] = @"TabView.png";
	textureNames[FIRE_TEXTURE] = @"Fire.jpg";
	textureNames[ASTEROID_TEXTURE] = @"Asteroid.png";
	textureNames[SHIELD_SPOT_TEXTURE] = @"ShieldSpot.png";
	textureNames[MENU_TEXTURE] = @"Menu.png";
	[self loadTextures:textureNames];
	shieldSpotTexture = texture[SHIELD_SPOT_TEXTURE];
	menuItemTex = texture[MENU_TEXTURE];
	size = 0;
	planetIndex = -1;
	shipIndex = -1;
	systemIndex = -1;
	messageTime = 10.;
	money = 100000;
	systems = [[NSMutableArray alloc] initWithCapacity:5];
	
	keyCode prefKey;
	
#define assignKey( control, name )	prefKey = fromName(name);	index[control] = prefKey.index;	val[control] = prefKey.val;
	
	assignKey( LEFT, "LEFT" )
	assignKey( RIGHT, "RIGHT" )
	assignKey( FORWARD, "W" )
	assignKey( SLOW, "S" )
	assignKey( AUTO, "SHIFT" )
	assignKey( SELECT, "TAB" )
	assignKey( ORBIT, "O" )
	assignKey( HYPER, "H" )
	assignKey( LAND, "L" )
	assignKey( PREF, "P" )
	assignKey( MAP, "M" )
	assignKey( SELECT_SHIP, "E" )
	assignKey( FIRE_PRIMARY, "MOUSE" )
	assignKey( SELECT_SYSTEM, "G" )
	assignKey( SELECT_SECONDARY, "/" )
	assignKey( FIRE_SECONDARY, "RT MOUSE" )
	assignKey( REAR_VIEW, "R" )
	assignKey( SELECT_AT, "," )
	assignKey( INVENTORY, "I" )
	assignKey( BOARD, "B" )
	assignKey( RECALL, "C" )
	assignKey( RD_PERSON, "3" )
	assignKey( PAUSE_KEY, "[" )
	assignKey( UP, "UP" )
	assignKey( DOWN, "DOWN" )
	assignKey( FULL, "F" )
	assignKey( THROTTLE_UP, "A" )
	assignKey( THROTTLE_DOWN, "Z" )
	assignKey( CONSOLE, "~" )
	assignKey( AB, "V" )
	assignKey( INERTIA, "N" )
	
	floatVal[MOUSE] = 1.0f;
	floatVal[INVERT_Y] = 0.0f;
	floatVal[SENSITIVITY] = 1.0f;

	glLightfv( GL_LIGHT1, GL_AMBIENT, ambient );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuse );
	glLightfv( GL_LIGHT1, GL_POSITION, lightPos );
	glEnable( GL_LIGHT1 );
	glEnable( GL_NORMALIZE );

	explode = [SoundManager registerSound:[[NSURL fileURLWithPath:@"Explosion.wav"] path]];
	itinerary = [[NSMutableArray alloc] initWithCapacity:6];
	shipCheckTime = 5.0;
	shipCheckDelta = 5.0;
	date = [[NSDate dateWithString:@"2250-01-01 00:00:00 +0000"] retain];
	
	NSLog( @"   load" );
	if( new ){
		flags = [[NSMutableDictionary alloc] init];
		[self save];
		[self load];
	} else {
		[self load];
	}
	state = ALIVE;
	
	interpreter = [[FSInterpreter alloc] init];
	[interpreter setObject:[NSBundle mainBundle] forIdentifier:@"MainBundle"];
	[interpreter execute:@"MainBundle load"];
	NSLog( @"   set player" );
	[interpreter setObject:self forIdentifier:@"player"];
	
	temp = [Controller objectsOfType:@"mission"];
	for( i = 0; i < [temp count]; i++ ){
		[(Mission*)[temp objectAtIndex:i] initialScript];
	}
	
	NSLog( @"   set up system" );
	[system setUp];
	
	[self initMenus];
	
	[self startThreads];

	NSLog( @"sys inited" );

	return self;
}

- (void) initPlugins{
	NSBundle *bundle;
	Class plugClass;
	id instance;
	NSEnumerator *fEnum;
	NSString *plugName;
	NSMutableArray *keys;
	int i, j;
	BOOL defaultPlugin = NO;
	int plugCount;
	extern float pluginLoading;
	NSArray *files;
	
	NSLog( @"   initing plugins" );
	plugins = [[NSMutableDictionary alloc] initWithCapacity:1];
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] bundlePath]];
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:@"../Plugins/"];
	fEnum = [[[NSFileManager defaultManager] directoryContentsAtPath:@"."] objectEnumerator];
	files = [fEnum allObjects];
	for( i = 0; i < [files count]; i++ ){
		plugName = [files objectAtIndex:i];
		if( [[plugName pathExtension] isEqualToString:@"bundle"] ){
			if( bundle = [NSBundle bundleWithPath:plugName] ){
				NSLog( @"   got plugin: %@", plugName );
				if( plugClass = [bundle principalClass] )
					instance = [[plugClass alloc] initWithController:self];
				else
					[self initDataFor:bundle];
			}
		}
		pluginLoading = i/(float)[files count]/2;
		drawPlayerScreen();
		aglSwapBuffers( aglGetCurrentContext() );
	}
	
	NSLog( @"   plugins loaded" );
	keys = [NSMutableArray arrayWithCapacity:[plugins count]];
	[keys addObjectsFromArray:[plugins allKeys]];
	if( defaultPlugin )
		[keys removeObject:@"default"];
	plugCount = [keys count];
	for( i = 0; i < plugCount-1; i++ ){
		if( i > 3 )
			break;
		for( j = i+1; j < plugCount; j++ ){
			if( [[keys objectAtIndex:i] compare:[keys objectAtIndex:j]] == NSOrderedDescending ){
				NSString *temp = [keys objectAtIndex:i];
				[keys replaceObjectAtIndex:i withObject:[keys objectAtIndex:j]];
				[keys replaceObjectAtIndex:j withObject:temp];
			}
		}
	}

	if( defaultPlugin )
		[keys insertObject:@"default" atIndex:0];
	NSLog( [keys description] );
	for( i = 0; i < [keys count]; i++ ){
		[self finalizePlugin:[plugins objectForKey:[keys objectAtIndex:i]]];
	}
}

- (void) initDataFor:(NSBundle *)plugin{
	NSFileManager *manager = [NSFileManager defaultManager];
	NSString *oldDirPath = [manager currentDirectoryPath];
	NSDictionary *info = [plugin infoDictionary];
	NSString *_name = [[info objectForKey:@"CFBundleExecutable"] lowercaseString];
	NSString *file;
	NSDirectoryEnumerator *enumerator;

	[manager changeCurrentDirectoryPath:[plugin resourcePath]];
	
	enumerator = [manager enumeratorAtPath:@"."];
	while( file = [enumerator nextObject] ){
		if( [[file pathExtension] isEqualToString:@"evh"] ){
			[self loadFile:file forPlugin:_name];
		}
	}
	
	[manager changeCurrentDirectoryPath:oldDirPath];
}

- (void) loadFile:(NSString *)file forPlugin:(NSString *)_name{
	CFStringRef errorString;
	NSMutableDictionary *dictionary = CFPropertyListCreateFromXMLData( NULL, (CFDataRef)[[NSFileManager defaultManager] contentsAtPath:file], kCFPropertyListImmutable, &errorString ); 
	if( dictionary ){
		NSString *type = [dictionary objectForKey:@"Type"];
		Class MyClass = NSClassFromString( type );
		[dictionary setObject:_name forKey:@"PluginName"];
		[MyClass registerFromDictionary:dictionary];
		totalFiles++;
	}
}


- (void) finalizePlugin:(id)plugin{
	if( [plugin isKindOfClass:[NSDictionary class]] ){
		NSEnumerator *enumerator = [plugin keyEnumerator];
		NSString *key;
		
		while (key = [enumerator nextObject]){
			if( ![key isEqualToString:@"start"] )
				[self finalizePlugin:[plugin objectForKey:key]];
		}
	} else {
		extern float pluginLoading;
//		NSLog( @"		finalizing object: %@", [plugin description] );
		[plugin finalize];
		filesDone++;
		pluginLoading = 0.5 + filesDone*0.5/totalFiles;
		drawPlayerScreen();
		aglSwapBuffers( aglGetCurrentContext() );
	}
}



- (void) setSystem:(Solarsystem *)_system{
	NSMutableArray *menuItems = [[NSMutableArray alloc] init];
	int i;
	
	[menuItems addObject:[[EHMenuItem alloc] initWithName:@"Jump" hotkey:keyValue( index[HYPER], val[HYPER] ) menu:nil width:128 target:self action:@selector(hyper)]];
	[menuItems addObject:[[EHMenuItem alloc] initWithName:@"Map" hotkey:keyValue( index[MAP], val[MAP] ) menu:nil width:128 target:self action:@selector(map)]];
	for( i = 0; i < [_system->links count]; i++ ){
		NSString *_name = ((Solarsystem*)[_system->links objectAtIndex:i])->name;
		const char *key = [[NSString stringWithFormat:@"%d", i+1] cString];
		[menuItems addObject:[[EHMenuItem alloc] initWithName:_name hotkey:key menu:nil width:128 target:self action:@selector(selectSystem:) object:_name]];
	}
	[EHMenu newMenuWithTitle:@"Hyperspace" items:menuItems];
	
	if( [itinerary count] > 0 && _system == [itinerary objectAtIndex:0] )
		[itinerary removeObjectAtIndex:0];
	if( [itinerary count] > 0 && [_system->links containsObject:[itinerary objectAtIndex:0]] )
		systemIndex = [_system->links indexOfObject:[itinerary objectAtIndex:0]];
	system = _system;
	[system setUp];
	[bg generate:.2];
}

- (void) update{
	KeyMap2 keys;
	int i;
	char *string;
	float theta = 180/pi*atan2( z+1000, x ) - angle;
	BOOL useMouse = YES;
	NSArray *temp;		// for temporary storage of objects to draw

	if( FACTOR != 0 )
		threadTime = FACTOR;
	_x = 1000;
	_y = 0;
	_z = 0;
	[self globalToLocal];
	lightPos[0] = _x;
	lightPos[1] = _y;
	lightPos[2] = _z;
	if( viewStyle == 1 ){		// overhead view lighting
		lightPos[0] = lightPos[0];
		lightPos[1] = -lightPos[2];
		lightPos[2] = 0;
	}
	glLightfv( GL_LIGHT1, GL_POSITION, lightPos );

	if( t > shipCheckTime ){
		AI *tempShip = [system->types newInstance];
		int delta = system->shipCount - [ships count];
		tempShip->x = 60-(120.0f*random())/RAND_MAX;
		tempShip->z = 60-(120.0f*random())/RAND_MAX;
		tempShip->y = 60-(120.0f*random())/RAND_MAX;
		[ships addObject:tempShip];
		if( delta > 0 )
			shipCheckDelta /= 1.5;
		else if( delta < 0 )
			shipCheckDelta *= 1.5;
		if( shipCheckDelta < 1 )
			shipCheckDelta = 1;
		shipCheckTime = t + (shipCheckDelta*random())/RAND_MAX;
	}
	
//	fprintf( stderr, "Controller update\n" );
#pragma mark Handle Keys
	getKeys( keys );
	
	killRot = YES;
	killVertRot = YES;

	if( [EHMenu active] ){
		goto updateKeys;	// don't collect input, menu is forefront
	}
	if( NEW_PRESS( CONSOLE ) ){
		console = !console;
		if( console )
			[Console setup];
		else
			[Console end];
	}
	if( console ){
		goto updateKeys;		// if in console, don't use keys
	}
#ifdef __ppc__
	if( keys[1] & 0x2000 && !(oldKeys[1] & 0x2000) ) // esc
#else
	if( keys[1] & 0x200000 && !(oldKeys[1] & 0x200000) ) // esc
#endif	
		[EHMenu displayMenu:@"Main Menu"];
	if( NEW_PRESS( PAUSE_KEY ) )
		[self pause];
	if( pause ){
		goto updateKeys;		// if paused, don't use keys
	}
	if( NEW_PRESS( PREF ) ){
		setUpPrefs();
		return;
	}
	if( keys[index[MAP]] & val[MAP] ){
		[self map];
		return;
	}
	if( keys[index[INVENTORY]] & val[INVENTORY] ){
		setUpInventory();
		return;
	}
	if( keys[index[LEFT]] & val[LEFT] ){	// left arrow
		useMouse = NO;
		left = YES;
		killRot = NO;
	} else if( oldKeys[index[LEFT]] & val[LEFT] ){
		killRot = YES;
	}
	if( keys[index[RIGHT]] & val[RIGHT] ){	// right arrow
		useMouse = NO;
		right = YES;
		killRot = NO;
	} else if( oldKeys[index[RIGHT]] & val[RIGHT] ){
		killRot = YES;
	}
	if( keys[index[FORWARD]] & val[FORWARD] )	// up arrow
		forward = YES;
	if( keys[index[SLOW]] & val[SLOW] )	// down arrow
		slow = YES;
	if( NEW_PRESS( AUTO ) )	// autopilot
		autopilot = !autopilot;
	if( NEW_PRESS( SELECT ) ){	// select next
		planetIndex++;
		if( [planets count] == planetIndex ){
			planetIndex = -1;
			curPlanet = NULL;
		} else {
			curPlanet = [planets objectAtIndex:planetIndex];
		}
	}
	if( NEW_PRESS( SELECT_SHIP ) ){
		shipIndex++;
		if( shipIndex < [ships count] && ([ships objectAtIndex:shipIndex] == sys || [sys->escorts containsObject:[ships objectAtIndex:shipIndex]]) )
			shipIndex++;
		if( [ships count] <= shipIndex ){
			shipIndex = -1;
			selection = NULL;
		} else
			selection = [ships objectAtIndex:shipIndex];
	}
	if( NEW_PRESS( SELECT_SYSTEM ) ){
		if( !hyper ){
			systemIndex++;
			if( [system->links count] <= systemIndex )
				systemIndex = -1;
		}
	}
	if( NEW_PRESS( ORBIT ) )	// orbit
		orbit = YES;
	if( NEW_PRESS( HYPER ) )	// hyperspace
		[EHMenu displayMenu:@"Hyperspace"];
		//hyper = YES;
	if( NEW_PRESS( LAND ) ){	// land
		land = YES;
	}
	if( keys[index[FIRE_PRIMARY]] & val[FIRE_PRIMARY] ){
		fire = YES;
	}
	if( NEW_PRESS( SELECT_SECONDARY ) ){
		secondaryIndex++;
		if( secondaryIndex >= [secondary count] )
			secondaryIndex = -1;
	}
	if( keys[index[FIRE_SECONDARY]] & val[FIRE_SECONDARY] ){
		fireSecondary = YES;
	}
	if( NEW_PRESS( REAR_VIEW ) ){
		if( viewStyle == 2 )
			viewStyle = 0;
		else
			viewStyle = 2;
	}
	if( NEW_PRESS( SELECT_AT ) ){
		int i;
		Spaceship *ship = nil;
		float dist = -1;

		for( i = 0; i < [ships count]; i++ ){
			Spaceship *temp = [ships objectAtIndex:i];
			float d;

			_x = temp->x;
			_y = temp->y;
			_z = temp->z;
			[self globalToLocal];
			if( _x < 0 || temp == sys || [sys->escorts containsObject:temp] )
				continue;
			d = (_z*_z+_y*_y)/(_x*_x);
			if( dist == -1 || d < dist ){
				dist = d;
				ship = temp;
			}
		}
		selection = ship;
		shipIndex = [ships indexOfObject:ship];
	}
	if( NEW_PRESS( BOARD ) && selection ){
		if( selection->state == DISABLED ){
			if( [self distance:selection] < (self->size+selection->size+0.25) )
				setUpBoard();
			else
				[self newMessage:@"You are too far away"];
		} else {
			[self newMessage:@"You can't board a live ship"];
		}
	}
	if( NEW_PRESS( RECALL ) ){
		bound = !bound;
		for( i = 0; i < [escorts count]; i++ ){
			AI *escort = [escorts objectAtIndex:i];
			escort->bound = bound;
			escort->goal = DO_ESCORT;
		}
	}
	if( NEW_PRESS( RD_PERSON ) ){
		if( viewStyle != 3 )
			viewStyle = 3;
		else
			viewStyle = 0;
	}
	if( keys[index[UP]] & val[UP] ){
		useMouse = NO;
		up = YES;
		killVertRot = NO;
	} else if( oldKeys[index[UP]] & val[UP] ){
		killVertRot = YES;
	}
	if( keys[index[DOWN]] & val[DOWN] ){
		useMouse = NO;
		down = YES;
		killVertRot = NO;
	} else if( oldKeys[index[DOWN]] & val[DOWN] ){
		killVertRot = YES;
	}
	if( NEW_PRESS( FULL ) )
		toggleFull = YES;
	if( keys[index[THROTTLE_UP]] & val[THROTTLE_UP] )
		throttleUp = YES;
	if( keys[index[THROTTLE_DOWN]] & val[THROTTLE_DOWN] )
		throttleDown = YES;
	if( keys[index[AB]] & val[AB] )
		afterburner = YES;
	if( NEW_PRESS( INERTIA ) )
		inertial = !inertial;

updateKeys:	
	
	oldKeys[0] = keys[0];
	oldKeys[1] = keys[1];
	oldKeys[2] = keys[2];
	oldKeys[3] = keys[3];
	
	#pragma mark Mouse

	if( useMouse && floatVal[MOUSE] == 0.0f && !autopilot ){
		Point mouse;
		float wantedRot;
		float wantedPitch;
		
		GetMouse( &mouse );
		if( !full ){
			extern NSWindow *window;
			NSRect r = [window frame];
			mouse.h -= r.origin.x;
			mouse.v = -r.origin.y+mouse.v;
		}
		wantedRot = floatVal[SENSITIVITY]*MAX_ANGULAR_VELOCITY*(screenWidth/2-mouse.h)/(screenWidth/2);
		wantedPitch = floatVal[SENSITIVITY]*MAX_ANGULAR_VELOCITY*(screenHeight/2-mouse.v)/(screenHeight/2);
		if( floatVal[INVERT_Y] == 1.0 )
			wantedPitch *= -1.0f;
		if( deltaRot < wantedRot ){		// mouse input as angular velocity indicator
			if( wantedRot-deltaRot <= ANGULAR_ACCELERATION*FACTOR )
				deltaRot = wantedRot;
			else
				left = YES;
		} else {
			if( deltaRot-wantedRot <= ANGULAR_ACCELERATION*FACTOR )
				deltaRot = wantedRot;
			else
				right = YES;
		}
		if( deltaPitch < wantedPitch ){
			if( wantedPitch-deltaPitch <= ANGULAR_ACCELERATION*FACTOR )
				deltaPitch = wantedPitch;
			else
				up = YES;
		} else if( deltaPitch > wantedPitch ){
			if( deltaPitch-wantedPitch <= ANGULAR_ACCELERATION*FACTOR )
				deltaPitch = wantedPitch;
			else
				down = YES;
		}
		killRot = NO;
		killVertRot = NO;
	}
	else if( useMouse && floatVal[MOUSE] == 1.0f && !autopilot ){
		int deltaX, deltaY;
		static float totalX = 0, totalY = 0;
		float wantedRot, wantedPitch;
		
		if( mouseMoved ){
			CGGetLastMouseDelta( &deltaX, &deltaY );
			mouseMoved = NO;
		} else {
			deltaX = 0;
			deltaY = 0;
		}

		totalX += deltaX/5.0;
		wantedRot = -totalX;
		totalX -= totalX*FACTOR*2;
		if( abs(totalX) > 5*MAX_ANGULAR_VELOCITY )
			totalX = 5*MAX_ANGULAR_VELOCITY*(totalX/abs(totalX));
		totalY += deltaY/5.0;
		wantedPitch = -totalY;
		totalY -= totalY*FACTOR*2;
		if( abs(totalY) > 5*MAX_ANGULAR_VELOCITY )
		totalY = 5*MAX_ANGULAR_VELOCITY*(totalY/abs(totalY));
		if( deltaRot < wantedRot ){
			if( wantedRot-deltaRot < ANGULAR_ACCELERATION*FACTOR )
				deltaRot = wantedRot;
			else
				left = YES;
		} else {
			if( deltaRot-wantedRot < ANGULAR_ACCELERATION*FACTOR )
				deltaRot = wantedRot;
			else
				right = YES;
		}
		if( floatVal[INVERT_Y] == 1.0f )
			wantedPitch *= -1.0f;
		if( deltaPitch < wantedPitch ){
			if( wantedPitch-deltaPitch < ANGULAR_ACCELERATION*FACTOR )
				deltaPitch = wantedPitch;
			else
				up = YES;
		} else {
			if( deltaPitch-wantedPitch < ANGULAR_ACCELERATION*FACTOR )
				deltaPitch = wantedPitch;
			else
				down = YES;
		}			
		killRot = NO;
		killVertRot = NO;			
	}

	[super update];

	if( killRot && !autopilot ){
	#define ACCEL_FACTOR 1
		if( deltaRot > 0 )
			deltaRot -= MAX_ANGULAR_VELOCITY*sys->FACTOR*ACCEL_FACTOR;
		else if( deltaRot < 0 )
			deltaRot += MAX_ANGULAR_VELOCITY*sys->FACTOR*ACCEL_FACTOR;
		if( sqr(deltaRot) < sqr(MAX_ANGULAR_VELOCITY*sys->FACTOR*ACCEL_FACTOR) )
			deltaRot = 0;
	} if( killVertRot && !autopilot ){
		if( deltaPitch > 0 )
			deltaPitch -= MAX_ANGULAR_VELOCITY*sys->FACTOR*ACCEL_FACTOR;
		else if( deltaPitch < 0 )
			deltaPitch += MAX_ANGULAR_VELOCITY*sys->FACTOR*ACCEL_FACTOR;
		if( sqr(deltaPitch) < sqr(MAX_ANGULAR_VELOCITY*sys->FACTOR*ACCEL_FACTOR) )
			deltaPitch = 0;
	}

#pragma mark  Draw Stuff
	glEnable( GL_BLEND );
	[bg draw];
	[system update];

	glEnable( GL_LIGHTING );
	for( i = 0; i < [planets count]; i++ ){		// draw each planet
		[(Planet *)[planets objectAtIndex:i] draw];
	}

	[shipLock lock];
	temp = [NSArray arrayWithArray:ships];
	[shipLock unlock];
	for( i = 0; i < [temp count]; i++ ){
		AI *ship = [temp objectAtIndex:i];
		if( ship->state != DEAD ){
			if( ship != self ){
				[ship draw];
			}
			else if( viewStyle == 1 || viewStyle == 3 ){
				[self position];
				glRotatef( 90, 0, 1, 0 );
				glScalef( size, size, size );
				glBindTexture( GL_TEXTURE_2D, escortee->texture[0] );
				[escortee drawObject];
			}
		}
	}

	[asteroidLock lock];
	temp = [NSArray arrayWithArray:asteroids];
	[asteroidLock unlock];
	for( i = 0; i < [temp count]; i++ ){
		[(Asteroid *)[temp objectAtIndex:i] draw];
	}
	
	[weaponLock lock];
	temp = [NSArray arrayWithArray:weaps];
	[weaponLock unlock];
	for( i = 0; i < [temp count]; i++ ){
		[(Weapon *)[temp objectAtIndex:i] draw];
	}

	glDisable( GL_LIGHTING );
	[EHMenu update:keys];

	if( curPlanet ){		// draw selection bracket
		glColor3f( 0, 1, 0 );
		[curPlanet bracket];
	}
	
	if( selection )
		[selection bracket];
	
	glDisable( GL_CULL_FACE ); // draw transparent stuff
	
	[shipLock lock];
	temp = [NSArray arrayWithArray:ships];
	[shipLock unlock];
	for( i = 0; i < [temp count]; i++ ){
		Spaceship *ship = [temp objectAtIndex:i];
		int j;
		[ship position];
		glRotatef( 90, 0, 1, 0 );
		for( j = 0; j < [ship->shieldSpots count]; j++ ){
			ShieldSpot *spot = [ship->shieldSpots objectAtIndex:j];
			[spot update:ship->shields/(float)ship->maxShield];
			if( spot->done )
				[ship->shieldSpots removeObjectAtIndex:j--];
		}
	}
	
	for( i = 0; i < [planets count]; i++ ){
		[[planets objectAtIndex:i] drawRing];
	}
	
	glEnable( GL_CULL_FACE );

	[StarDust update];

	glDisable( GL_DEPTH_TEST );
	if( viewStyle == 0 || viewStyle == 2 )
		[self drawFrame];
	if( sys->viewStyle != 2 ){
		[self drawNavTab];
		[self drawSelectionTab:1];
		[self drawSelectionTab:2];
		[self drawSelectionTab:3];
		[self drawInfoTab];
	}
	if( console )
		[Console draw];
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );

	if( viewStyle != 2 ){
		glColor3f( 0, 1, 0 );
		if( selection ){	// draw ship stats
			string = [[NSString stringWithFormat:@"Shield: %d%% (%d)", 100*(int)selection->shields/selection->maxShield, (int)selection->shields] cString];
			[self drawString:string atX:5/6.+0.01 y:1.*(screenHeight-screenWidth/6-7.6*screenWidth/30+15)/screenHeight];
			string = [[NSString stringWithFormat:@"Armor:  %d%% (%d)", 100*selection->armor/selection->maxArmor, selection->armor] cString];
			[self drawString:string atX:5/6.+0.01 y:1.*(screenHeight-screenWidth/6-7.6*screenWidth/30)/screenHeight];
			string = [selection->name cString];
		}
		else
			string = "<No Ship Selected>";
		[self drawString:string atX:5/6.+0.01 y:1.*(screenHeight-screenWidth/6-3.6*screenWidth/30)/screenHeight];
		if( selection )
			[self drawString:[selection->gov->name cString] atX:5/6.+0.01 y:1.*(screenHeight-screenWidth/6-4*screenWidth/30)/screenHeight];
		if( curPlanet ){		// draw planet name
			if( centerOfRotation == curPlanet )
				string = [[NSString stringWithFormat:@"%@ <ORB>", curPlanet->name] cString];
			else string = [curPlanet->name cString];
		} else string = "<No Planet Selected>";
		[self drawString:string atX:5/6.+0.01 y:1.*(screenHeight-screenWidth/6-2.6*screenWidth/30)/screenHeight];
		if( x*x+z*z+y*y < jumpDistance && [system->planets count] > 0 )
			glColor3f( 0, .5, 0 );
		if( systemIndex == -1 )
			string = "<No System Selected>";
		else{
			if( systemIndex < 0 )
				NSLog( @"systemIndex = %d", systemIndex );
			else if( systemIndex >= [system->links count] )
				NSLog( @"systemIndex = %d, exceeds limit = %d", systemIndex, [system->links count] );
			string = [((Solarsystem*)[system->links objectAtIndex:systemIndex])->name cString];
		}
		[self drawString:string atX:5/6.+0.01 y:1.*(screenHeight-screenWidth/6-1.6*screenWidth/30)/screenHeight];
		glColor3f( 0, 1, 0 );
		if( secondaryIndex == -1 )
			string = "<No Secondary Weapon>";
		else{
			Weapon *weap = ((Mod*)[secondary objectAtIndex:secondaryIndex])->ammo;
			NSString *_name = [weap name];
			int count = 0;
			if( weap = [self hasMod:weap] )
				count = [weap count];
			string = [[NSString stringWithFormat:@"%@: %d", _name, count] cString];
		}
		[self drawString:string atX:5/6.+0.01 y:1.*(screenHeight-screenWidth/6-.6*screenWidth/30)/screenHeight];
		if( !pause )
			[self drawString:[[NSString stringWithFormat:@"FPS = %d", (int)(1./FACTOR)] cString] atX:0.005 y:0.95];
	}

	if( messageTime < 5 && message ){
		if( full )
			[self drawString:[message cString] atX:0.005 y:0.01];
		else
			[self drawString:[message cString] atX:0.005 y:22./sys->screenHeight+.01];
		messageTime += FACTOR;
	}
	
	if( shields <= 0 && armor <= 0 )
		state = DEAD;

	[SoundManager update];
}

+ componentNamed:(NSString *)name{
	NSMutableArray *path = [name componentsSeparatedByString:@"."];
	id dict = sys->plugins;
	int i;
	
	for( i = 0; i < [path count]; i++ ){
		dict = [dict objectForKey:[path objectAtIndex:i]];
		if( dict == nil )
			continue;
	}

	return dict;
}

+ (NSMutableDictionary *) ensurePlugin:(NSString *)pluginName path:(NSString *)pathName{
	NSMutableDictionary *plugin = [sys->plugins objectForKey:pluginName];
	NSMutableDictionary *path;
	
	if( !plugin ){
		plugin = [[NSMutableDictionary alloc] initWithCapacity:1];
		[sys->plugins setObject:plugin forKey:pluginName];
	}
	
	path = [plugin objectForKey:pathName];
	if( !path ){
		[plugin setObject:[[NSMutableDictionary alloc] initWithCapacity:1] forKey:pathName];
		path = [plugin objectForKey:pathName];
	}
	
	return path;
}

- (NSMutableDictionary *) getPlugins{
	return plugins;
}

- (void) hyper{
	if( fuel < 1 ){
		hyper = NO;
		[self newMessage:@"Not enough fuel"];
		return;
	}
	if( systemIndex == -1 ){
		hyper = NO;
		[self newMessage:@"No system selected"];
		return;
	}
	if( x*x+z*z+y*y < jumpDistance && [system->planets count] > 0 ){
		hyper = NO;
		glDisable( GL_FOG );
		if( hyperTime == 0.0f )
			[self newMessage:@"You are too close to external mass"];
		else
			[self newMessage:@"Jump aborted: nearby mass"];
		hyperTime = 0.0f;
		return;
	}
	hyper = YES;
	if( hyperTime == 0 ){
		glEnable( GL_FOG );
		glFogf( GL_FOG_END, 2 );
	}
	hyperTime += sys->FACTOR;
	glColor4f( 0, 0, 0, hyperTime );
	glFogf( GL_FOG_DENSITY, hyperTime );
	if( hyperTime > 5.0f ){
		extern BOOL dirtyMissions;
		int i;
		Solarsystem *temp = [system->links objectAtIndex:systemIndex];
		float dist;
		glDisable( GL_FOG );
		hyperTime = 0;
		hyper = NO;
		curPlanet = nil;
		centerOfRotation = nil;
		autopilot = NO;
		planetIndex = -1;
		selection = nil;
		shipIndex = -1;
		systemIndex = -1;
		x = system->x - temp->x;
		z = system->z - temp->z;
		dist = sqrt(x*x + z*z);
		speedx = -x*MAX_VELOCITY/dist;
		speedz = -z*MAX_VELOCITY/dist;
		speedy = 0;
		x = 1.2*sqrt(jumpDistance)*x/dist;
		z = 1.2*sqrt(jumpDistance)*z/dist;
		y = 0;
		angle = atan2( speedz, speedx )*180/pi - 90;
		roll = 0;
		pitch = 0;
		[self setSystem:temp];
		fuel--;
		dirtyMissions = YES;
		[date autorelease];
		date = [[date addTimeInterval:86400.0] retain];
		for( i = 0; i < [missions count]; i++ ){
			[[missions objectAtIndex:i] update];
		}
	}
}

- (void) land{
	if( curPlanet ){
		if( sqrt( sqr(x-curPlanet->x) + sqr(z-curPlanet->z) + sqr(y-curPlanet->y) ) <= curPlanet->size+1+size ){
			if( sys->curPlanet->model )
				[CutScene setup:LAND_STATION];
			else
				[CutScene setup:LAND_1];
			messageTime = 10.;
			shields = maxShield;
			armor = maxArmor;
			autopilot = NO;
			[date autorelease];
			date = [[date addTimeInterval:86400.0] retain];
			[self save];
		} else {
			[self newMessage:[NSString stringWithFormat:@"<%@ Control>: You are too far away", curPlanet->name]];
		}
	}
}

- (int) setShip:(Spaceship *)temp{
	NSEnumerator *keys;
	id key;
	int amountCargo = 0, amountMods = 0, i;
if( !temp )
	NSLog( @"setShip:NIL!!!" );
	ANGULAR_ACCELERATION = temp->ANGULAR_ACCELERATION;
	MAX_ANGULAR_VELOCITY = temp->MAX_ANGULAR_VELOCITY;
	ACCELERATION = temp->ACCELERATION;
	MAX_VELOCITY = temp->MAX_VELOCITY;
	self->ID = [temp->ID retain];
	mass = temp->mass;
	cargoSpace = temp->cargoSpace;
	price = temp->price;
	shields = temp->shields;
	maxShield = shields;
	armor = temp->armor;
	maxArmor = armor;
	thrust = temp->thrust;
	size = temp->size;
	fuel = temp->fuel;
	maxFuel = fuel;
	primaryStraight = temp->primaryStraight;
	primaryTurret = temp->primaryTurret;
	secondaryStraight = temp->secondaryStraight;
	secondaryTurret = temp->secondaryTurret;
	rechargeRate = temp->rechargeRate;
	escortee = temp;
	
	modSpace = 0;
	keys = [temp->initData keyEnumerator];
	while( key = [keys nextObject] ){
		int num = [[temp->initData objectForKey:key] intValue];
		int i;
		for( i = 0; i < num; i++ ){
			[self addMod:[Controller componentNamed:key]];
			amountMods += [[Controller componentNamed:key] mass];
		}
	}
	modSpace = temp->modSpace;
	for( i = 0; i < [cargo count]; i++ ){
		Cargo *c = [cargo objectAtIndex:i];
		amountCargo += c->size;
	}
	while( amountCargo > cargoSpace ){
		Cargo *c = [cargo lastObject];
		NSLog( @"%d", c->size );
		if( amountCargo - c->size >= cargoSpace ){
			[cargo removeLastObject];
			amountCargo -= c->size;
		} else{
			c->size -= amountCargo-cargoSpace;
			amountCargo = cargoSpace;
		}
	}
	cargoSpace -= amountCargo;
	
	frameFile = temp->frameFile;
	[self setUpFrame];
	
	return amountMods + modSpace;
}

- (void) save{
	NSMutableDictionary *data = [[NSMutableDictionary alloc] init];
	CFDataRef xml;
	int i;
	NSMutableArray *array = [NSMutableArray arrayWithCapacity:[cargo count]];
	NSMutableDictionary *modData = [[[NSMutableDictionary alloc] init] autorelease];
	
	[data setObject:system->ID forKey:@"System"];
	[data setObject:[NSNumber numberWithInt:money] forKey:@"Money"];
	if( self->ID ){
		[data setObject:self->ID forKey:@"Ship"];
	} else
	for( i = 0; i < [cargo count]; i++ ){
		Cargo *c = [cargo objectAtIndex:i];
		NSNumber *num = [NSNumber numberWithInt:c->size];
		NSMutableDictionary *temp = [[[NSMutableDictionary alloc] init] autorelease];
		[temp setObject:c->name forKey:@"Name"];
		[temp setObject:num forKey:@"Size"];
		[array addObject:temp];
	}
	[data setObject:array forKey:@"Cargo"];
	array = [NSMutableArray arrayWithCapacity:END_OF_KEYS];
	for( i = 0; i < END_OF_KEYS; i++ ){
		[array addObject:[NSString stringWithCString:keyValue( index[i], val[i] )]];
	}
	[data setObject:array forKey:@"Controls"];
	array = [NSMutableArray arrayWithCapacity:END_OF_FLOATS];
	for( i = 0; i < END_OF_FLOATS; i++ ){
		[array addObject:[NSNumber numberWithInt:floatVal[i]]];
	}
	[data setObject:array forKey:@"Floats"];
	for( i = 0; i < [weapons count]; i++ ){
		[modData setObject:[NSNumber numberWithInt:[[weapons objectAtIndex:i] count]] forKey:[[weapons objectAtIndex:i] ID]];
	}
	for( i = 0; i < [mods count]; i++ ){
		[modData setObject:[NSNumber numberWithInt:[[mods objectAtIndex:i] count]] forKey:[[mods objectAtIndex:i] ID]];
	}
	[data setObject:modData forKey:@"Mods"];
	array = [NSMutableArray arrayWithCapacity:[itinerary count]];
	for( i = 0; i < [itinerary count]; i++ ){
		[array addObject:((Solarsystem*)[itinerary objectAtIndex:i])->ID];
	}
	[data setObject:array forKey:@"Itinerary"];
	array = [NSMutableArray arrayWithCapacity:[missions count]];
	for( i = 0; i < [missions count]; i++ ){
		NSMutableDictionary *temp = [NSMutableDictionary dictionaryWithCapacity:8];
		Mission *mission = [missions objectAtIndex:i];
		[temp setObject:mission->endSystem->ID forKey:@"EndSystem"];
		[temp setObject:mission->endPlanet->ID forKey:@"EndPlanet"];
		[temp setObject:[NSNumber numberWithInt:mission->size] forKey:@"Size"];
		[temp setObject:[NSNumber numberWithInt:mission->payment] forKey:@"Payment"];
		[temp setObject:mission->title forKey:@"Title"];
		[temp setObject:mission->endText forKey:@"EndText"];
		if( mission->expires )
			[temp setObject:mission->expires forKey:@"Expires"];
		[temp setObject:mission->ID forKey:@"ID"];
		[array addObject:temp];
	}
	[data setObject:array forKey:@"Missions"];
	[data setObject:[NSNumber numberWithFloat:fuel] forKey:@"Fuel"];
	[data setObject:flags forKey:@"Flags"];
	[data setObject:date forKey:@"Date"];
	[data setObject:govRecord forKey:@"GovRecord"];
	[data setObject:systems forKey:@"Systems"];
	xml = CFPropertyListCreateXMLData( NULL, data );	
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] bundlePath]];
	[[NSFileManager defaultManager] createFileAtPath:[NSString stringWithFormat:@"../Players/%@.evh", name] contents:xml attributes:nil];	
	[data release];
	CFRelease( xml );
}

- (void) load{
	CFStringRef errorString;
	NSMutableDictionary *dictionary;
	NSDictionary *tempDict;
	NSArray *array;
	NSEnumerator *keys;
	id key;
	int i;

	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] bundlePath]];
	dictionary = CFPropertyListCreateFromXMLData( NULL, [[NSFileManager defaultManager] contentsAtPath:[NSString stringWithFormat:@"../Players/%@.evh", name]], kCFPropertyListImmutable, &errorString );
	modSpace = [self setShip:[Controller componentNamed:[dictionary objectForKey:@"Ship"]]];
	[mods removeAllObjects];
	[weapons removeAllObjects];
	[secondary removeAllObjects];
	array = [dictionary objectForKey:@"Cargo"];
	for( i = 0; i < [array count]; i++ ){
		Cargo *c = [[Cargo alloc] init];
		NSDictionary *temp = [array objectAtIndex:i];
		c->name = [[NSString alloc] initWithString:[temp objectForKey:@"Name"]];
		c->size = [[temp objectForKey:@"Size"] intValue];
		[cargo addObject:c];
		cargoSpace -= c->size;
		mass += c->size;
	}
	array = [dictionary objectForKey:@"Controls"];
	for( i = 0; i < [array count]; i++ ){
		keyCode key = fromName( [(NSString *)[array objectAtIndex:i] cString] );
		val[i] = key.val;
		index[i] = key.index;
	}
	array = [dictionary objectForKey:@"Floats"];
	for( i = 0; i < [array count]; i++ ){
		floatVal[i] = [[array objectAtIndex:i] floatValue];
	}
	tempDict = [dictionary objectForKey:@"Mods"];
	keys = [tempDict keyEnumerator];
	while( key = [keys nextObject] ){
		int i = [[tempDict objectForKey:key] intValue];
		int num;
		for( num = 0; num < i; num++ ){
			if( [key isEqualToString:@"special.weapons.Forklift"] ){
				[Forklift setup];
				[Forklift install];
			} else if( [key isEqualToString:@"special.mods.ForkliftLauncher"] )
				;
			else
				[self addMod:[Controller componentNamed:key]];
		}
	}
	array = [dictionary objectForKey:@"Itinerary"];
	for( i = 0; i < [array count]; i++ ){
		[itinerary addObject:[Controller componentNamed:[array objectAtIndex:i]]];
	}
	array = [dictionary objectForKey:@"Missions"];
	for( i = 0; i < [array count]; i++ ){
		NSDictionary *temp = [array objectAtIndex:i];
		Mission *mission = [[Mission alloc] init];
		mission->endSystem = [Controller componentNamed:[temp objectForKey:@"EndSystem"]];
		mission->endPlanet = [Controller componentNamed:[temp objectForKey:@"EndPlanet"]];
		mission->size = [[temp objectForKey:@"Size"] intValue];
		mission->payment = [[temp objectForKey:@"Payment"] intValue];
		mission->title = [[temp objectForKey:@"Title"] retain];
		mission->endText = [[temp objectForKey:@"EndText"] retain];
		mission->expires = [temp objectForKey:@"Expires"];
		if( mission->expires )
			[mission->expires retain];
		mission->ID = [[temp objectForKey:@"ID"] retain];
		mission->setFlags = ((Mission*)[Controller componentNamed:mission->ID])->setFlags;
		mission->endScript = ((Mission*)[Controller componentNamed:mission->ID])->endScript;
		[missions addObject:mission];
		cargoSpace -= mission->size;
		mass += mission->size;
	}
	ACCELERATION = thrust/(mass*compensation);
	[self setSystem:[Controller componentNamed:[dictionary objectForKey:@"System"]]];
	money = [[dictionary objectForKey:@"Money"] intValue];
	fuel = [[dictionary objectForKey:@"Fuel"] floatValue];
	flags = [[NSMutableDictionary dictionaryWithDictionary:[dictionary objectForKey:@"Flags"]] retain];
	[date release];
	date = [[dictionary objectForKey:@"Date"] retain];
	[govRecord release];
	govRecord = [[dictionary objectForKey:@"GovRecord"] retain];
	[systems setArray:[dictionary objectForKey:@"Systems"]];
	for( i = 0; i < [systems count]; i++ )
		((Solarsystem*)[Controller componentNamed:[systems objectAtIndex:i]])->displayOnMap = YES;
	[dictionary release];
}

- (BOOL) functionKey{
	KeyMap2 keys;
	getKeys( keys );
	if( keys[index[MAP]] & val[MAP] )
		setUpMap();
	else if( keys[index[INVENTORY]] & val[INVENTORY] )
		setUpInventory();
	else
		return NO;
	return YES;
}

+ (NSMutableArray *) objectsOfType:(NSString *)type{
	NSMutableArray *objects = [NSMutableArray arrayWithCapacity:5];
	int i;
	NSArray *temp = [sys->plugins allValues];
	for( i = 0; i < [temp count]; i++ ){
		NSDictionary *d2, *d = [temp objectAtIndex:i];
		d2 = [d objectForKey:[NSString stringWithFormat:@"%@s", type]];
		if( d2 ){
			[objects addObjectsFromArray:[d2 allValues]];
		}
		if( [type isEqualToString:@"mod"] ){
			d2 = [d objectForKey:@"weapons"];
			if( d2 )
				[objects addObjectsFromArray:[d2 allValues]];
		}
	}
	return objects;
}

+ (NSMutableArray *) objectsOfType:(NSString *)type withFlags:(NSString *)flags{
	return [Controller objects:[Controller objectsOfType:type] withFlags:flags];
}

+ (NSMutableArray *) objects:(NSArray *)temp withFlags:(NSString *)flags{
	NSArray *args = flags ? [flags componentsSeparatedByString:@" "] : [NSArray array];
	NSMutableArray *stack = [[NSMutableArray alloc] initWithCapacity:20];
	NSString *val;
	NSMutableArray *objects = [NSMutableArray arrayWithCapacity:20];
	int i;

	if( [args count] == 0 )
		return temp;

	for( i = 0; i < [temp count]; i++ ){
		NSDictionary *objectFlags = [[temp objectAtIndex:i] flags];
		int j;

		for( j = 0; j < [args count]; j++ ){
			val = [args objectAtIndex:j];
			if( [val isEqualToString:@"OR"] ){
				int arg1 = [[stack lastObject] intValue];
				int arg2;
				[stack removeLastObject];
				arg2 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 || arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else if( [val isEqualToString:@"AND"] ){
				int arg1 = [[stack lastObject] intValue];
				int arg2;
				[stack removeLastObject];
				arg2 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 && arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];				
			} else if( [val isEqualToString:@"NOT"] ){
				int arg = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg == 0 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else if( [val isEqualToString:@">"] ){
				int arg2 = [[stack lastObject] intValue];
				int arg1;
				[stack removeLastObject];
				arg1 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 > arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else if( [val isEqualToString:@">="] ){
				int arg2 = [[stack lastObject] intValue];
				int arg1;
				[stack removeLastObject];
				arg1 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 >= arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else if( [val isEqualToString:@"<"] ){
				int arg2 = [[stack lastObject] intValue];
				int arg1;
				[stack removeLastObject];
				arg1 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 < arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else if( [val isEqualToString:@"<="] ){
				int arg2 = [[stack lastObject] intValue];
				int arg1;
				[stack removeLastObject];
				arg1 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 <= arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else if( [val isEqualToString:@"=="] ){
				int arg2 = [[stack lastObject] intValue];
				int arg1;
				[stack removeLastObject];
				arg1 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 == arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else if( [val isEqualToString:@"!="] ){
				int arg2 = [[stack lastObject] intValue];
				int arg1;
				[stack removeLastObject];
				arg1 = [[stack lastObject] intValue];
				[stack removeLastObject];
				if( arg1 != arg2 )
					[stack addObject:[NSNumber numberWithInt:1]];
				else
					[stack addObject:[NSNumber numberWithInt:0]];
			} else {
				if( [val rangeOfString:@"$"].location == 0 )
					[stack addObject:[NSNumber numberWithInt:[sys valForFlag:val]]];
				else if( [objectFlags objectForKey:val] ){
					[stack addObject:[objectFlags objectForKey:val]];
				}
				else
					[stack addObject:[NSNumber numberWithInt:[val intValue]]];
			}
		}
		
		if( [[stack lastObject] intValue] )
			[objects addObject:[temp objectAtIndex:i]];
		[stack removeLastObject];
	}

	return objects;
}


- (void) dealloc{
	int i;

	[name release];
	[bg release];
	[ships release];
	[weaps release];
	[asteroids release];
	[missions release];
	[cargo release];
	[itinerary release];
	[date release];
	[flags release];
	for( i = 0; i < 16; i++ ){
//		[chan[i] release];
	}
	
	[super dealloc];
}

@end