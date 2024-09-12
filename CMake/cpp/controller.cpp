#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>

#include <freetype-gl/freetype-gl.h>
#include <freetype-gl/vertex-buffer.h>
#include <freetype-gl/vertex-attribute.h>

#include "shader.h"

#include "controller.h"
#include "ai.h"
#include "plugin.h"
#include "ehobject.h"
#include "mission.h"
#include "government.h"
#include "solarsystem.h"
#include "spaceship.h"
#include "mod.h"
#include "weapon.h"
#include "type.h"

#define sqr(x) ((x)*(x))

using json = nlohmann::json;

void (*drawScene)(void);
void (*eventScene)(SDL_Event&);

static void toLower(string& str);

void add_text(GLfloat* vertices, GLuint* indices, texture_font_t* font, const string& str, float x, float y);

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

texture_atlas_t* atlas;
texture_font_t* font;

string Controller::basePath;
GLShaderProgram* Controller::stringShader;
GLShaderProgram* Controller::shader;

void Controller::initialize() {
	basePath = filesystem::current_path().string() + "/";
	atlas = texture_atlas_new(128, 128, 1);
	font = texture_font_new_from_file(atlas, 16, (Controller::basePath + "Resources/ChicagoFLF.ttf").c_str());
	char* str = new char[97];
	for (int i = 0; i < 96; i++)
		str[i] = i + 32;
	str[96] = 0;
	texture_font_load_glyphs(font, str);
	glGenTextures(1, &atlas->id);
	glBindTexture(GL_TEXTURE_2D, atlas->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas->width, atlas->height, 0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);

	string stringVertSource = "#version 330 core\n"
		"layout(location = 0) in vec3 vertex;\n"
		"layout(location = 1) in vec2 tex_coord;\n"
		"uniform mat4 projection;\n"
		"out vec2 TexCoords;\n"
		"void main() {\n"
		"	TexCoords = tex_coord;\n"
		"	gl_Position = projection * vec4(vertex, 1.0);\n"
		"	TexCoords = tex_coord;\n"
		"}\n";

	string stringFragSource = "#version 330 core\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"uniform vec3 textColor;\n"
		"in vec2 TexCoords;\n"
		"void main() {\n"
		"	vec4 sample = texture(text, TexCoords);\n"
		"	color = vec4(textColor, sample.r);\n"
		"}\n";
	stringShader = new GLShaderProgram(stringVertSource, stringFragSource);

	string shaderVertSource = "#version 330 core\n"
		"layout(location = 0) in vec3 vertex;\n"
		"layout(location = 1) in vec3 normal;\n"
		"layout(location = 2) in vec2 tex_coord;\n"
		"uniform mat4 projection;\n"
		"uniform mat4 view;\n"
		"uniform mat4 model;\n"
		"out vec2 TexCoords;\n"
		"out vec3 FragPos;\n"
		"out vec3 Normal;\n"
		"void main() {\n"
		"	TexCoords = tex_coord;\n"
		"	Normal = normal;\n"
		"	FragPos = vec3(model * vec4(vertex, 1.0));\n"
		"	gl_Position = projection * view * vec4(FragPos, 1.0);\n"
		"}\n";
	string shaderFragSource = "#version 330 core\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"in vec2 TexCoords;\n"
		"in vec3 Normal;\n"
		"in vec3 FragPos;\n"
		"void main() {\n"
		// ambient
		"	float ambientStrength = 0.1;"
		"	vec3 ambient = vec3(ambientStrength);"
		// diffuse
		"	vec3 norm = normalize(Normal);"
		"	vec3 lightDir = normalize(vec3(1, 0, 0));"
		"	float diff = max(dot(norm, lightDir), 0.0);"
		"	vec3 diffuse = diff * vec3(1.0);"
		// specular
		"	float specularStrength = 0.5;"
		"	vec3 viewDir = normalize(vec3(0, 0, 3) - FragPos);"
		"	vec3 reflectDir = reflect(-lightDir, norm);"
		"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
		"	vec3 specular = specularStrength * spec * vec3(1.0);"
		// result
		//"	vec4 sample = texture(text, TexCoords);\n"
		"	color = vec4(ambient + diffuse + specular, 1.0);\n"
		"}\n";
	shader = new GLShaderProgram(shaderVertSource, shaderFragSource);
}

void Controller::initPlugins() {
	// list directories in basePath
	filesystem::path path = basePath + "Plugins";
	vector<string> files;
	for (const auto& entry : filesystem::directory_iterator(path)) {
		if (entry.is_directory())
			files.push_back(entry.path().string());
	}
	// for each directory, load data
	for (const string& file : files) {
		string plugin = file.substr(file.find_last_of(filesystem::path::preferred_separator) + 1);
		initDataFor(plugin);
	}
}

void Controller::initDataFor(const string& plugin) {
	string lower = plugin;
	toLower(lower);
	for (const auto& entry : filesystem::recursive_directory_iterator(basePath + "Plugins/" + plugin)) {
		if (entry.is_regular_file()) {
			filesystem::path path = entry.path();
			if (path.extension() == ".json" || path.extension() == ".JSON")
				loadFileForPlugin(path, lower);
		}
	}
}

void Controller::loadFileForPlugin(const filesystem::path& filePath, const string& pluginName) {
	ifstream inF(filePath.string());
	json data = json::parse(inF);
	if (!data.contains("Type"))
		return;
	string typeName = data["Type"];
	toLower(typeName);
	data["PluginName"] = pluginName;
	filesystem::path directory = filePath;
	data["Directory"] = directory.remove_filename().string();
	if( !data.contains("ID") )
		data["ID"] = filePath.stem().string();
	if (typeName == "spaceship") {
		Spaceship::registerFromDictionary(data);
	}
	else if (typeName == "mod") {
		Mod::registerFromDictionary(data);
	}
	else if (typeName == "weapon") {
		Weapon::registerFromDictionary(data);
	}
	else if (typeName == "mission") {
		Mission::registerFromDictionary(data);
	}
	else if (typeName == "government") {
		Government::registerFromDictionary(data);
	}
	else if (typeName == "solarsystem") {
		Solarsystem::registerFromDictionary(data);
	} else if( typeName == "planet" ) {
		Planet::registerFromDictionary(data);
	} else if( typeName == "type" ) {
		Type::registerFromDictionary(data);
	}
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
	initPlugins();
	if (isNew) {
		setSystem(static_pointer_cast<Solarsystem>(componentNamed("default.systems.start")));
		setShip(static_pointer_cast<Spaceship>(componentNamed("default.ships.start")));
		tm m_date;
		m_date.tm_year = 350;
		m_date.tm_mon = 0;
		m_date.tm_mday = 1;
		m_date.tm_hour = 0;
		m_date.tm_min = 0;
		m_date.tm_sec = 0;
		date = chrono::system_clock::from_time_t(mktime(&m_date));
	}
	//((map<string, Solarsystem*>*)componentNamed("systems"))->erase("start");
	//((map<string, Spaceship*>*)componentNamed("ships"))->erase("start");
	// change to default plugin resource path
	textureNames[NAVPANEL_TEXTURE] = Controller::basePath + "Images/TabView2.png";
	textureNames[NAVPLANET_TEXTURE] = Controller::basePath + "Images/NavPlanet.png";
	textureNames[WIDGET_TEXTURE] = Controller::basePath + "Images/Widgets.png";
	textureNames[INFOTAB_TEXTURE] = Controller::basePath + "Images/TabView.png";
	textureNames[FIRE_TEXTURE] = Controller::basePath + "Images/Fire.png";
	textureNames[ASTEROID_TEXTURE] = Controller::basePath + "Images/Asteroid.png";
	textureNames[SHIELD_SPOT_TEXTURE] = Controller::basePath + "Images/ShieldSpot.png";
	textureNames[MENU_TEXTURE] = Controller::basePath + "Images/Menu.png";
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

void Controller::setSystem(shared_ptr<Solarsystem> system) {
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

	for ( shared_ptr<Spaceship> ship : ships ) {
		shared_ptr<Spaceship> ship = ships[i];
		if (ship->state != DEAD) {
			if (ship.get() != this) {
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

	for (shared_ptr<Asteroid> asteroid : asteroids ) {
		asteroid->draw();
	}

	for (shared_ptr<Weapon> weap : weapons ) {
		//weap->draw();
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

	for ( shared_ptr<Spaceship> ship : ships ) {
		ship->position();
		glRotatef(90, 0, 1, 0);
		for (int j = 0; j < ship->shieldSpots.size(); j++) {
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

vector<shared_ptr<EHObject>> Controller::objectsOfType(const string& type) {
	vector<shared_ptr<EHObject>> temp;
	for( auto& [key, value] : Plugin::plugins ) {
		if( type == "mission" ){
			for( auto& [key2, value2] : value->missions ) {
				temp.push_back(value2);
			}
		}
		else if( type == "government" ) {
			for( auto& [key2, value2] : value->governments ) {
				temp.push_back(value2);
			}
		}
		else if( type == "system" ) {
			for( auto& [key2, value2] : value->solarsystems ) {
				temp.push_back(value2);
			}
		}
		else if( type == "ship" ) {
			for( auto& [key2, value2] : value->spaceships ) {
				temp.push_back(value2);
			}
		}
	}
	return temp;
}

shared_ptr<EHObject> Controller::componentNamed(const string& name) {
	vector<string> parts = split(name, '.');
	shared_ptr<Plugin> plugin;
	if( Plugin::plugins.count(parts[0]) == 0 )
		return nullptr;
	plugin = Plugin::plugins[parts[0]];
	if (parts[1] == "systems") {
		if( plugin->solarsystems.count(parts[2]) == 0 )
			return nullptr;
		return plugin->solarsystems[parts[2]];
	}
	else if( parts[1] == "ships" ){
		if( plugin->spaceships.count(parts[2]) == 0 )
			return nullptr;
		return plugin->spaceships[parts[2]];
	}
	else if( parts[1] == "governments" ){
		if( plugin->governments.count(parts[2]) == 0 )
			return nullptr;
		return plugin->governments[parts[2]];
	}
	else if (parts[1] == "missions") {
		if (plugin->missions.count(parts[2]) == 0)
			return nullptr;
		return plugin->missions[parts[2]];
	}
	else if (parts[1] == "planets") {
		if (plugin->planets.count(parts[2]) == 0)
			return nullptr;
		return plugin->planets[parts[2]];
	}
	return nullptr;
}

void Controller::setShip(shared_ptr<Spaceship> ship) {

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

void Controller::drawString(const string& str, float x, float y, float *color) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	stringShader->use();
	stringShader->setUniformMat4("projection", glm::value_ptr(GLShaderProgram::orthoTransform));
	stringShader->setUniform3fv("textColor", color);

	GLfloat *vertices = new GLfloat[20 * str.size()];
	GLuint *indices = new GLuint[6 * str.size()];

	add_text(vertices, indices, font, str, x, y);

	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*20*str.size(), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6*str.size(), indices, GL_STATIC_DRAW);

	glBindTexture(GL_TEXTURE_2D, atlas->id);

	glDrawElements(GL_TRIANGLES, 6*str.length(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	delete[] vertices;
	delete[] indices;
}

void add_text(GLfloat* vertices, GLuint* indices, texture_font_t* font, const string& str, float x, float y) {
	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
	float penX = x, penY = y;
	const char* text = str.c_str();
	for (int i = 0; i < str.length(); i++) {
		int vOffset = i * 20;
		int iOffset = i * 6;
		texture_glyph_t* glyph = texture_font_get_glyph(font, text + i);
		if (glyph != nullptr) {
			float kerning = 0.0f;
			if (i > 0) {
				kerning = texture_glyph_get_kerning(glyph, text + i - 1);
			}
			penX += kerning;
			float x0 = penX + glyph->offset_x;
			float y0 = penY + glyph->offset_y;
			float x1 = x0 + glyph->width;
			float y1 = y0 - glyph->height;
			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;
			GLuint indicesCur[6] = { i*4, i*4+1, i*4+2, i*4, i*4+2, i*4+3 };
			GLfloat verticesCur[4 * 5] = {
				x0, y0, 0, s0, t0,
				x0, y1, 0, s0, t1,
				x1, y1, 0, s1, t1,
				x1, y0, 0, s1, t0
			};
			memcpy(vertices + vOffset, verticesCur, 4 * 5 * sizeof(GLfloat));
			memcpy(indices + iOffset, indicesCur, 6 * sizeof(GLuint));
			penX += glyph->advance_x;
		}
	}
}

void drawGLScene() {

}

static void toLower(string& str) {
	for (int i = 0; i < str.size(); i++)
		str[i] = tolower(str[i]);
}

vector<string> split(const string& str, char c) {
	vector<string> result;
	string::size_type start = 0;
	string::size_type end = 0;
	while ((end = str.find(c, start)) != string::npos) {
		result.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	result.push_back(str.substr(start));
	return result;
}

string replace(const string& str, const string& old, const string& replace) {
	string result = str;
	string::size_type pos = 0;
	while ((pos = result.find(old, pos)) != string::npos) {
		result.replace(pos, old.size(), replace);
		pos += replace.size();
	}
	return result;
}