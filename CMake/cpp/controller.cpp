#include <SDL2/SDL.h>
#include <fstream>
#include <SDL_filesystem.h>

#include <freetype-gl/freetype-gl.h>
#include <freetype-gl/vertex-buffer.h>
#include <freetype-gl/vertex-attribute.h>
#include "shader.h"

#include "controller.h"
#include "ai.h"

#define sqr(x) ((x)*(x))

void (*drawScene)(void);

void add_text(vertex_buffer_t* buffer, texture_font_t* font, const char* text, vec4* color, vec2* pen);

extern const Uint8* keyMap;
extern Uint8* oldKeys;
extern int oldMouseX, oldMouseY;

bool killRot = false;
bool killVertRot = false;
GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat diffuse[] = { 1, 1, 1, 1 };
GLfloat lightPos[] = { 0, 0, 0, 1 };

int explode;

int totalFiles = 0;
int filesDone = 0;

bool debug = false;

texture_atlas_t* atlas = texture_atlas_new(512, 512, 1);
texture_font_t* font;

string vert_source = "/* Freetype GL - A C OpenGL Freetype engine\
*\
*Distributed under the OSI - approved BSD 2 - Clause License.See accompanying\
* file `LICENSE` for more details.\
*/\
uniform mat4 model;\
uniform mat4 view;\
uniform mat4 projection;\
\
attribute vec3 vertex;\
attribute vec2 tex_coord;\
attribute vec4 color;\
void main()\
{\
	gl_TexCoord[0].xy = tex_coord.xy;\
	gl_FrontColor = color;\
	gl_Position = projection * (view * (model * vec4(vertex, 1.0)));\
}";
string frag_source = "/* Freetype GL - A C OpenGL Freetype engine\
*\
*Distributed under the OSI - approved BSD 2 - Clause License.See accompanying\
* file `LICENSE` for more details.\
*/\
uniform sampler2D texture;\
void main()\
{\
	float a = texture2D(texture, gl_TexCoord[0].xy).r;\
	gl_FragColor = vec4(gl_Color.rgb, gl_Color.a * a);\
}";

GLuint shader;

string Controller::basePath;

void Controller::initialize() {
	//shader = load_shader(vert_source.c_str(), frag_source.c_str());
	char *temp = SDL_GetBasePath();
	basePath = temp;
	SDL_free(temp);
	font = texture_font_new_from_file(atlas, 24, (Controller::basePath + "Resources/ChicagoFLF.ttf").c_str());
}

Controller::Controller(string name, bool isNew) {
	vector<string> textureNames(NUM_TEXTURES);
	int i;
	vector<void*> temp;
	extern GLuint shieldSpotTexture, menuItemTex;

	ID = "";
	sys = this;
	this->name = name;
	//bg = new Background();
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
	texture.resize(texNum);
	//initPlugins();
	if (isNew) {
		setSystem((Solarsystem*)componentNamed("default.systems.start"));
		setShip((Spaceship*)componentNamed("default.ships.start"));
	}
	((map<string, Solarsystem*>*)componentNamed("systems"))->erase("start");
	((map<string, Spaceship*>*)componentNamed("ships"))->erase("start");
	// change to default plugin resource path
	textureNames[NAVPANEL_TEXTURE] = "TabView2.png";
	textureNames[NAVPLANET_TEXTURE] = "NavPlanet.tiff";
	textureNames[WIDGET_TEXTURE] = "Widgets.png";
	textureNames[INFOTAB_TEXTURE] = "TabView.png";
	textureNames[FIRE_TEXTURE] = "Fire.jpg";
	textureNames[ASTEROID_TEXTURE] = "Asteroid.png";
	textureNames[SHIELD_SPOT_TEXTURE] = "ShieldSpot.png";
	textureNames[MENU_TEXTURE] = "Menu.png";
	loadTextures(textureNames);
	shieldSpotTexture = texture[SHIELD_SPOT_TEXTURE];
	menuItemTex = texture[MENU_TEXTURE];
	size = 0;
	planetIndex = -1;
	shipIndex = -1;
	systemIndex = -1;
	messageTime = 10.;
	money = 100000;

/*
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
*/

	shipCheckTime = 5.0;
	shipCheckDelta = 5.0;

	//this->system->setUp();
	//initMenus();
	//startThreads();
}

void Controller::setSystem(Solarsystem* system) {
	this->system = system;
	int i;

	/*
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
	*/

	system->setUp();
}

void Controller::update() {
	// KeyMap2 keys;
	int i;
	string str;
	double theta = 180 / pi * atan2(z + 1000, x) - angle;
	bool useMouse = true;
	vector<SpaceObject*> temp;		// temporary storage of objects to draw

	/*if (FACTOR != 0)
		threadTime = FACTOR;*/
	_x = 1000;
	_y = 0;
	_z = 0;
	globalToLocal();
	lightPos[0] = _x;
	lightPos[1] = _y;
	lightPos[2] = _z;
	if (viewStyle == 1) {		// overhead view lighting
		lightPos[0] = lightPos[0];
		lightPos[1] = -lightPos[2];
		lightPos[2] = 0;
	}
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

/*	if (t > shipCheckTime) {
		AI* tempShip = [system->types newInstance];
		int delta = system->shipCount - [ships count];
		tempShip->x = 60 - (120.0f * random()) / RAND_MAX;
		tempShip->z = 60 - (120.0f * random()) / RAND_MAX;
		tempShip->y = 60 - (120.0f * random()) / RAND_MAX;
		[ships addObject : tempShip] ;
		if (delta > 0)
			shipCheckDelta /= 1.5;
		else if (delta < 0)
			shipCheckDelta *= 1.5;
		if (shipCheckDelta < 1)
			shipCheckDelta = 1;
		shipCheckTime = t + (shipCheckDelta * random()) / RAND_MAX;
	}*/

	/*
	//	fprintf( stderr, "Controller update\n" );
#pragma mark Handle Keys
	getKeys(keys);

	killRot = YES;
	killVertRot = YES;

	if ([EHMenu active]) {
		goto updateKeys;	// don't collect input, menu is forefront
	}
	if (NEW_PRESS(CONSOLE)) {
		console = !console;
		if (console)
			[Console setup];
		else
			[Console end];
	}
	if (console) {
		goto updateKeys;		// if in console, don't use keys
	}
#ifdef __ppc__
	if (keys[1] & 0x2000 && !(oldKeys[1] & 0x2000)) // esc
#else
	if (keys[1] & 0x200000 && !(oldKeys[1] & 0x200000)) // esc
#endif	
		[EHMenu displayMenu : @"Main Menu"];
	if (NEW_PRESS(PAUSE_KEY))
		[self pause];
	if (pause) {
		goto updateKeys;		// if paused, don't use keys
	}
	if (NEW_PRESS(PREF)) {
		setUpPrefs();
		return;
	}
	if (keys[index[MAP]] & val[MAP]) {
		[self map] ;
		return;
	}
	if (keys[index[INVENTORY]] & val[INVENTORY]) {
		setUpInventory();
		return;
	}
	if (keys[index[LEFT]] & val[LEFT]) {	// left arrow
		useMouse = NO;
		left = YES;
		killRot = NO;
	}
	else if (oldKeys[index[LEFT]] & val[LEFT]) {
		killRot = YES;
	}
	if (keys[index[RIGHT]] & val[RIGHT]) {	// right arrow
		useMouse = NO;
		right = YES;
		killRot = NO;
	}
	else if (oldKeys[index[RIGHT]] & val[RIGHT]) {
		killRot = YES;
	}
	if (keys[index[FORWARD]] & val[FORWARD])	// up arrow
		forward = YES;
	if (keys[index[SLOW]] & val[SLOW])	// down arrow
		slow = YES;
	if (NEW_PRESS(AUTO))	// autopilot
		autopilot = !autopilot;
	if (NEW_PRESS(SELECT)) {	// select next
		planetIndex++;
		if ([planets count] == planetIndex) {
			planetIndex = -1;
			curPlanet = NULL;
		}
		else {
			curPlanet = [planets objectAtIndex : planetIndex];
		}
	}
	if (NEW_PRESS(SELECT_SHIP)) {
		shipIndex++;
		if (shipIndex < [ships count] && ([ships objectAtIndex : shipIndex] == sys || [sys->escorts containsObject : [ships objectAtIndex : shipIndex] ]))
			shipIndex++;
		if ([ships count] <= shipIndex) {
			shipIndex = -1;
			selection = NULL;
		}
		else
			selection = [ships objectAtIndex : shipIndex];
	}
	if (NEW_PRESS(SELECT_SYSTEM)) {
		if (!hyper) {
			systemIndex++;
			if ([system->links count] <= systemIndex)
				systemIndex = -1;
		}
	}
	if (NEW_PRESS(ORBIT))	// orbit
		orbit = YES;
	if (NEW_PRESS(HYPER))	// hyperspace
		[EHMenu displayMenu : @"Hyperspace"];
	//hyper = YES;
	if (NEW_PRESS(LAND)) {	// land
		land = YES;
	}
	if (keys[index[FIRE_PRIMARY]] & val[FIRE_PRIMARY]) {
		fire = YES;
	}
	if (NEW_PRESS(SELECT_SECONDARY)) {
		secondaryIndex++;
		if (secondaryIndex >= [secondary count])
			secondaryIndex = -1;
	}
	if (keys[index[FIRE_SECONDARY]] & val[FIRE_SECONDARY]) {
		fireSecondary = YES;
	}
	if (NEW_PRESS(REAR_VIEW)) {
		if (viewStyle == 2)
			viewStyle = 0;
		else
			viewStyle = 2;
	}
	if (NEW_PRESS(SELECT_AT)) {
		int i;
		Spaceship* ship = nil;
		float dist = -1;

		for (i = 0; i < [ships count]; i++) {
			Spaceship* temp = [ships objectAtIndex : i];
			float d;

			_x = temp->x;
			_y = temp->y;
			_z = temp->z;
			[self globalToLocal] ;
			if (_x < 0 || temp == sys || [sys->escorts containsObject : temp])
				continue;
			d = (_z * _z + _y * _y) / (_x * _x);
			if (dist == -1 || d < dist) {
				dist = d;
				ship = temp;
			}
		}
		selection = ship;
		shipIndex = [ships indexOfObject : ship];
	}
	if (NEW_PRESS(BOARD) && selection) {
		if (selection->state == DISABLED) {
			if ([self distance : selection] < (self->size + selection->size + 0.25))
				setUpBoard();
			else
				[self newMessage : @"You are too far away"];
		}
		else {
			[self newMessage : @"You can't board a live ship"] ;
		}
	}
	if (NEW_PRESS(RECALL)) {
		bound = !bound;
		for (i = 0; i < [escorts count]; i++) {
			AI* escort = [escorts objectAtIndex : i];
			escort->bound = bound;
			escort->goal = DO_ESCORT;
		}
	}
	if (NEW_PRESS(RD_PERSON)) {
		if (viewStyle != 3)
			viewStyle = 3;
		else
			viewStyle = 0;
	}
	if (keys[index[UP]] & val[UP]) {
		useMouse = NO;
		up = YES;
		killVertRot = NO;
	}
	else if (oldKeys[index[UP]] & val[UP]) {
		killVertRot = YES;
	}
	if (keys[index[DOWN]] & val[DOWN]) {
		useMouse = NO;
		down = YES;
		killVertRot = NO;
	}
	else if (oldKeys[index[DOWN]] & val[DOWN]) {
		killVertRot = YES;
	}
	if (NEW_PRESS(FULL))
		toggleFull = YES;
	if (keys[index[THROTTLE_UP]] & val[THROTTLE_UP])
		throttleUp = YES;
	if (keys[index[THROTTLE_DOWN]] & val[THROTTLE_DOWN])
		throttleDown = YES;
	if (keys[index[AB]] & val[AB])
		afterburner = YES;
	if (NEW_PRESS(INERTIA))
		inertial = !inertial;
		*/
updateKeys:

	if (oldKeys != nullptr) {
		delete[] oldKeys;
	}
	oldKeys = new Uint8[sizeof(keyMap)];
	memcpy(oldKeys, keyMap, sizeof(keyMap));

	if (useMouse && floatVal[MOUSE] == 0.0f && !autopilot) {
		int mouseX, mouseY;
		float wantedRot;
		float wantedPitch;

		SDL_GetMouseState(&mouseX, &mouseY);
		/*if (!full) {
			extern NSWindow* window;
			NSRect r = [window frame];
			mouse.h -= r.origin.x;
			mouse.v = -r.origin.y + mouse.v;
		}*/
		wantedRot = floatVal[SENSITIVITY] * MAX_ANGULAR_VELOCITY * (screenWidth / 2 - mouseX) / (screenWidth / 2);
		wantedPitch = floatVal[SENSITIVITY] * MAX_ANGULAR_VELOCITY * (screenHeight / 2 - mouseY) / (screenHeight / 2);
		if (floatVal[INVERT_Y] == 1.0)
			wantedPitch *= -1.0f;
		if (deltaRot < wantedRot) {		// mouse input as angular velocity indicator
			if (wantedRot - deltaRot <= ANGULAR_ACCELERATION * FACTOR)
				deltaRot = wantedRot;
			else
				left = true;
		}
		else {
			if (deltaRot - wantedRot <= ANGULAR_ACCELERATION * FACTOR)
				deltaRot = wantedRot;
			else
				right = true;
		}
		if (deltaPitch < wantedPitch) {
			if (wantedPitch - deltaPitch <= ANGULAR_ACCELERATION * FACTOR)
				deltaPitch = wantedPitch;
			else
				up = true;
		}
		else if (deltaPitch > wantedPitch) {
			if (deltaPitch - wantedPitch <= ANGULAR_ACCELERATION * FACTOR)
				deltaPitch = wantedPitch;
			else
				down = true;
		}
		killRot = false;
		killVertRot = false;
	}
	else if (useMouse && floatVal[MOUSE] == 1.0f && !autopilot) {
		int deltaX, deltaY;
		static float totalX = 0, totalY = 0;
		float wantedRot, wantedPitch;

		SDL_GetRelativeMouseState(&deltaX, &deltaY);	// get mouse movement
		if (deltaX != oldMouseX || deltaY != oldMouseY ) {
			deltaX -= oldMouseX;
			deltaY -= oldMouseY;
			oldMouseX += deltaX;
			oldMouseY += deltaY;
		}
		else {
			deltaX = 0;
			deltaY = 0;
		}

		totalX += deltaX / 5.0;
		wantedRot = -totalX;
		totalX -= totalX * FACTOR * 2;
		if (abs(totalX) > 5 * MAX_ANGULAR_VELOCITY)
			totalX = 5 * MAX_ANGULAR_VELOCITY * (totalX / abs(totalX));
		totalY += deltaY / 5.0;
		wantedPitch = -totalY;
		totalY -= totalY * FACTOR * 2;
		if (abs(totalY) > 5 * MAX_ANGULAR_VELOCITY)
			totalY = 5 * MAX_ANGULAR_VELOCITY * (totalY / abs(totalY));
		if (deltaRot < wantedRot) {
			if (wantedRot - deltaRot < ANGULAR_ACCELERATION * FACTOR)
				deltaRot = wantedRot;
			else
				left = true;
		}
		else {
			if (deltaRot - wantedRot < ANGULAR_ACCELERATION * FACTOR)
				deltaRot = wantedRot;
			else
				right = true;
		}
		if (floatVal[INVERT_Y] == 1.0f)
			wantedPitch *= -1.0f;
		if (deltaPitch < wantedPitch) {
			if (wantedPitch - deltaPitch < ANGULAR_ACCELERATION * FACTOR)
				deltaPitch = wantedPitch;
			else
				up = true;
		}
		else {
			if (deltaPitch - wantedPitch < ANGULAR_ACCELERATION * FACTOR)
				deltaPitch = wantedPitch;
			else
				down = true;
		}
		killRot = true;
		killVertRot = true;
	}

	Spaceship::update();

	if (killRot && !autopilot) {
#define ACCEL_FACTOR 1
		if (deltaRot > 0)
			deltaRot -= MAX_ANGULAR_VELOCITY * sys->FACTOR * ACCEL_FACTOR;
		else if (deltaRot < 0)
			deltaRot += MAX_ANGULAR_VELOCITY * sys->FACTOR * ACCEL_FACTOR;
		if (sqr(deltaRot) < sqr(MAX_ANGULAR_VELOCITY * sys->FACTOR * ACCEL_FACTOR))
			deltaRot = 0;
	} if (killVertRot && !autopilot) {
		if (deltaPitch > 0)
			deltaPitch -= MAX_ANGULAR_VELOCITY * sys->FACTOR * ACCEL_FACTOR;
		else if (deltaPitch < 0)
			deltaPitch += MAX_ANGULAR_VELOCITY * sys->FACTOR * ACCEL_FACTOR;
		if (sqr(deltaPitch) < sqr(MAX_ANGULAR_VELOCITY * sys->FACTOR * ACCEL_FACTOR))
			deltaPitch = 0;
	}

// Draw Stuff
	glEnable(GL_BLEND);
	//[bg draw] ;
	system->update();

	glEnable(GL_LIGHTING);
	for (i = 0; i < planets.size(); i++) {		// draw each planet
		planets[i]->draw();
	}

	for (i = 0; i < ships.size(); i++) {
		Spaceship* ship = ships[i];
		if (ship->state != DEAD) {
			if (ship != this) {
				ship->draw();
			}
			else if (viewStyle == 1 || viewStyle == 3) {
				position();
				glRotatef(90, 0, 1, 0);
				glScalef(size, size, size);
				glBindTexture(GL_TEXTURE_2D, escortee->texture[0]);
				escortee->drawObject();
			}
		}
	}

	for (i = 0; i < asteroids.size(); i++) {
		asteroids[i]->draw();
	}

	for (i = 0; i < weaps.size(); i++) {
		//weaps[i]->draw();
	}

	glDisable(GL_LIGHTING);
	//[EHMenu update : keys] ;

	if (curPlanet) {		// draw selection bracket
		glColor3f(0, 1, 0);
		curPlanet->bracket();
	}

	if (selection)
		selection->bracket();

	glDisable(GL_CULL_FACE); // draw transparent stuff

	for (i = 0; i < ships.size(); i++) {
		Spaceship* ship = ships[i];
		int j;
		ship->position();
		glRotatef(90, 0, 1, 0);
		for (j = 0; j < ship->shieldSpots.size(); j++) {
			/*ShieldSpot* spot = ship->shieldSpots[j];
			spot->update(ship->shields / (float)ship->maxShield);
			if (spot->done) {
				ship->shieldSpots.erase(ship->shieldSpots.begin() + j--);
			}*/
		}
	}

	for (i = 0; i < planets.size(); i++) {
		//planets[i]->drawRing();
	}

	glEnable(GL_CULL_FACE);

	//[StarDust update] ;

	glDisable(GL_DEPTH_TEST);
	//if (viewStyle == 0 || viewStyle == 2)
	//	drawFrame();
	//if (sys->viewStyle != 2) {
	//	drawNavTab();
	//	drawSelectionTab(1);
	//	drawSelectionTab(2);
	//	drawSelectionTab(3);
	//	drawInfoTab();
	//}
	/*if (console)
		[Console draw];*/
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//if (viewStyle != 2) {
	//	glColor3f(0, 1, 0);
	//	if (selection) {	// draw ship stats
	//		string = [[NSString stringWithFormat : @"Shield: %d%% (%d)", 100 * (int)selection->shields / selection->maxShield, (int)selection->shields]cString];
	//		[self drawString : string atX : 5 / 6. + 0.01 y : 1. * (screenHeight - screenWidth / 6 - 7.6 * screenWidth / 30 + 15) / screenHeight] ;
	//		string = [[NSString stringWithFormat : @"Armor:  %d%% (%d)", 100 * selection->armor / selection->maxArmor, selection->armor]cString];
	//		[self drawString : string atX : 5 / 6. + 0.01 y : 1. * (screenHeight - screenWidth / 6 - 7.6 * screenWidth / 30) / screenHeight] ;
	//		string = [selection->name cString];
	//	}
	//	else
	//		string = "<No Ship Selected>";
	//	[self drawString : string atX : 5 / 6. + 0.01 y : 1. * (screenHeight - screenWidth / 6 - 3.6 * screenWidth / 30) / screenHeight] ;
	//	if (selection)
	//		[self drawString : [selection->gov->name cString] atX : 5 / 6. + 0.01 y : 1. * (screenHeight - screenWidth / 6 - 4 * screenWidth / 30) / screenHeight];
	//	if (curPlanet) {		// draw planet name
	//		if (centerOfRotation == curPlanet)
	//			string = [[NSString stringWithFormat : @"%@ <ORB>", curPlanet->name]cString];
	//		else string = [curPlanet->name cString];
	//	}
	//	else string = "<No Planet Selected>";
	//	[self drawString : string atX : 5 / 6. + 0.01 y : 1. * (screenHeight - screenWidth / 6 - 2.6 * screenWidth / 30) / screenHeight] ;
	//	if (x * x + z * z + y * y < jumpDistance && [system->planets count] > 0)
	//		glColor3f(0, .5, 0);
	//	if (systemIndex == -1)
	//		string = "<No System Selected>";
	//	else {
	//		if (systemIndex < 0)
	//			NSLog(@"systemIndex = %d", systemIndex);
	//		else if (systemIndex >= [system->links count])
	//			NSLog(@"systemIndex = %d, exceeds limit = %d", systemIndex, [system->links count]);
	//		string = [((Solarsystem*)[system->links objectAtIndex : systemIndex])->name cString];
	//	}
	//	[self drawString : string atX : 5 / 6. + 0.01 y : 1. * (screenHeight - screenWidth / 6 - 1.6 * screenWidth / 30) / screenHeight];
	//	glColor3f(0, 1, 0);
	//	if (secondaryIndex == -1)
	//		string = "<No Secondary Weapon>";
	//	else {
	//		Weapon* weap = ((Mod*)[secondary objectAtIndex : secondaryIndex])->ammo;
	//		NSString* _name = [weap name];
	//		int count = 0;
	//		if (weap = [self hasMod : weap])
	//			count = [weap count];
	//		string = [[NSString stringWithFormat : @"%@: %d", _name, count]cString];
	//	}
	//	[self drawString : string atX : 5 / 6. + 0.01 y : 1. * (screenHeight - screenWidth / 6 - .6 * screenWidth / 30) / screenHeight];
	//	if (!pause)
	//		[self drawString : [[NSString stringWithFormat : @"FPS = %d", (int)(1. / FACTOR)]cString] atX : 0.005 y : 0.95];
	//}

	/*if (messageTime < 5 && message) {
		if (full)
			[self drawString : [message cString] atX : 0.005 y : 0.01];
		else
			[self drawString : [message cString] atX : 0.005 y : 22. / sys->screenHeight + .01];
		messageTime += FACTOR;
	}*/

	if (shields <= 0 && armor <= 0)
		state = DEAD;

	//[SoundManager update] ;
}

void* Controller::componentNamed(string name) {
	return nullptr;
}

void Controller::setShip(Spaceship* ship) {

}

void Controller::doHyperspace(){
}

void Controller::doLand(){
}

void Controller::setUpFrame() {
	if (frameFront != 0) {
		glDeleteLists(frameFront, 2);
	}
	frameFront = glGenLists(2);
	frameRear = frameFront + 1;
	if( frameFile == "" ){
		glNewList(frameFront, GL_COMPILE);
		glBegin(GL_QUADS);
		glColor3f(.5, .5, .5);
		glVertex2f(-.83, 1);
		glVertex2f(-.83, .555);
		glColor3f(0, 0, 0);
		glVertex2f(-.67, .555);
		glVertex2f(-.67, 1);

		glVertex2f(-1, -1);
		glVertex2f(-.67, .555);
		glColor3f(.5, .5, .5);
		glVertex2f(-.83, .555);
		glVertex2f(-1.15, -1);

		glVertex2f(1.15, -1);
		glVertex2f(.83, .567);
		glColor3f(0, 0, 0);
		glVertex2f(.67, .567);
		glVertex2f(1, -1);

		glVertex2f(.67, 1);
		glVertex2f(.67, .567);
		glColor3f(.5, .5, .5);
		glVertex2f(.83, .567);
		glVertex2f(.83, 1);
		glEnd();
		glEndList();
		frameRear = frameFront;
	} else {
		ifstream file;
		file.open(frameFile);
		string str ;

		if (file.is_open()) {
			while (file >> str && !file.eof()) {
				if (str == "<FRONT>")
					glNewList(frameFront, GL_COMPILE);
				else if (str == "<REAR>")
					glNewList(frameRear, GL_COMPILE);
				else if (str == "<REARSAME>")
					frameRear = frameFront;
				else if (str == "</FRONT>" || str == "</REAR>")
					glEndList();

				else if( str == "<QUADS>" )
					glBegin(GL_QUADS);
				else if (str == "<POINTS>")
					glBegin(GL_POINTS);
				else if (str == "<LINES>")
					glBegin(GL_LINES);
				else if (str == "<LINESTRIP>")
					glBegin(GL_LINE_STRIP);
				else if (str == "<LINELOOP>")
					glBegin(GL_LINE_LOOP);
				else if (str == "<TRIANGLES>")
					glBegin(GL_TRIANGLES);
				else if (str == "<TRIANGLESTRIP>")
					glBegin(GL_TRIANGLE_STRIP);
				else if (str == "<TRIANGLEFAN>")
					glBegin(GL_TRIANGLE_FAN);
				else if (str == "<QUADSTRIP>")
					glBegin(GL_QUAD_STRIP);
				else if (str == "<POLYGON>")
					glBegin(GL_POLYGON);
				else if (str == "</QUADS>" || str == "</POINTS>" || str == "</LINES>" || str == "</LINESTRIP>" || str == "</LINELOOP>" || str == "</TRIANGLES>" || str == "</TRIANGLESTRIP>" || str == "</TRIANGLEFAN>" || str == "</QUADSTRIP>" || str == "</POLYGON>")
					glEnd();

				else if (str == "Color") {
					float f1, f2, f3;
					file >> f1 >> f2 >> f3;
					glColor3f(f1, f2, f3);
				}
				else if (str == "Vertex") {
					float f1, f2;
					file >> f1 >> f2;
					glVertex2f(f1, f2);
				}
			}
			file.close();
		}
	}
}

void drawString(string str, float x, float y) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, gScreenWidth, 0, gScreenHeight, 0, 1);
	glMatrixMode(GL_MODELVIEW);

	glRasterPos2f(gScreenWidth * x, gScreenHeight * y);
	vertex_buffer_t* buffer = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");
	texture_font_load_glyphs(font, str.c_str());
	vec4 color = { {1, 1, 1, 1} }; // RGBA white
	vec2 pen = { {x*gScreenWidth, y*gScreenHeight} }; // Starting position
	add_text(buffer, font, str.c_str(), &color, &pen);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, atlas->id);

	glUseProgram(shader);
	{
		glUniform1i(glGetUniformLocation(shader, "texture"),
			0);
		//glUniformMatrix4fv(glGetUniformLocation(shader, "model"),
		//	1, 0, model.data);
		//glUniformMatrix4fv(glGetUniformLocation(shader, "view"),
		//	1, 0, view.data);
		//glUniformMatrix4fv(glGetUniformLocation(shader, "projection"),
		//	1, 0, projection.data);
		vertex_buffer_render(buffer, GL_TRIANGLES);
	}

	vertex_buffer_delete(buffer);

	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// ------------------------------------------------------- typedef & struct ---
typedef struct {
	float x, y, z;    // position
	float s, t;       // texture
	float r, g, b, a; // color
} vertex_t;

// --------------------------------------------------------------- add_text ---
void add_text(vertex_buffer_t* buffer, texture_font_t* font,
	const char* text, vec4* color, vec2* pen)
{
	size_t i;
	float r = color->red, g = color->green, b = color->blue, a = color->alpha;
	for (i = 0; i < strlen(text); ++i)
	{
		texture_glyph_t* glyph = texture_font_get_glyph(font, text + i);
		if (glyph != NULL)
		{
			float kerning = 0.0f;
			if (i > 0)
			{
				kerning = texture_glyph_get_kerning(glyph, text + i - 1);
			}
			pen->x += kerning;
			int x0 = (int)(pen->x + glyph->offset_x);
			int y0 = (int)(pen->y + glyph->offset_y);
			int x1 = (int)(x0 + glyph->width);
			int y1 = (int)(y0 - glyph->height);
			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;
			GLuint indices[6] = { 0,1,2, 0,2,3 };
			vertex_t vertices[4] = { { x0,y0,0,  s0,t0,  r,g,b,a },
									 { x0,y1,0,  s0,t1,  r,g,b,a },
									 { x1,y1,0,  s1,t1,  r,g,b,a },
									 { x1,y0,0,  s1,t0,  r,g,b,a } };
			vertex_buffer_push_back(buffer, vertices, 4, indices, 6);
			pen->x += glyph->advance_x;
		}
	}
}

void drawGLScene() {

}