#include "solarsystem.h"
#include "spaceobject.h"
#include "controller.h"
#include "texture.h"
#include "ai.h"

GLuint backdrop[4];
int backdropWidth[4];
int backdropHeight[4];

void Solarsystem::setUp() {
	int i, j = 0;

	displayOnMap = true;
	SpaceObject::sys->selection = nullptr;
	SpaceObject::sys->shipIndex = -1;
	SpaceObject::sys->curPlanet = nullptr;
	SpaceObject::sys->planetIndex = -1;
	SpaceObject::sys->enemies.clear();
	SpaceObject::sys->ships.clear();
	SpaceObject::sys->ships.push_back(SpaceObject::sys);
	SpaceObject::sys->weaps.clear();
	SpaceObject::sys->asteroids.clear();
	for (i = 0; i < planets.size(); i++) {
		Planet* planet = planets[i];
		if (!planet->model) {
			Texture::loadTexture(planet->texFile.c_str(), &SpaceObject::sys->planetTex[i], nullptr, nullptr);
			planet->texture[0] = SpaceObject::sys->planetTex[i];
			if (planet->ringSize != 0) {
				Texture::loadTexture(planet->ringTex.c_str(), &SpaceObject::sys->planetTex[planets.size() + j], nullptr, nullptr);
				planet->texture[1] = SpaceObject::sys->planetTex[planets.size() + j++];
			}
			if (planet->atmosSize != 0) {
				Texture::loadTexture(planet->atmosTex.c_str(), &SpaceObject::sys->planetTex[planets.size() + j], nullptr, nullptr);
				planet->texture[2] = SpaceObject::sys->planetTex[planets.size() + j++];
			}
		}
	}
	// autorelease old planets
	SpaceObject::sys->planets = planets;
	/*for (i = 0; i < shipCount; i++) {
		AI* tempShip = [types newInstance];
		tempShip->x = 60.0f - (120.0f * random()) / RAND_MAX;
		tempShip->z = 60.0f - (120.0f * random()) / RAND_MAX;
		tempShip->y = 60.0f - (120.0f * random()) / RAND_MAX;
		[SpaceObject::sys->ships addObject : tempShip] ;
	}*/
	for (i = 0; i < asteroids; i++) {
		SpaceObject::sys->asteroids.push_back(new Asteroid());
	}
	for (i = 0; i < backdrops; i++) {
		Texture::loadTexture(backdropPath[i].c_str(), &backdrop[i], &backdropWidth[i], &backdropHeight[i]);
	}
	if (!SpaceObject::sys->systems.count(ID))
		SpaceObject::sys->systems.emplace(ID);
}

void Solarsystem::finalize() {
	//int i, count = [initData count];
	//NSMutableArray* temp;

	////NSLog( @"Finalizing system: %@", name );
	//for (i = 0; i < count - 3; i++) {	// load planets
	//	//NSLog( @"\tadding planet: %@", [initData objectAtIndex:i] );
	//	[planets addObject : [Controller componentNamed : [initData objectAtIndex : i] ] ] ;
	//}

	//temp = [initData objectAtIndex : count - 3];	// load links
	//for (i = 0; i < [temp count]; i++) {
	//	Solarsystem* s;
	//	//NSLog( @"\tadding link: %@", [temp objectAtIndex:i] );
	//	s = [Controller componentNamed : [temp objectAtIndex : i] ];
	//	if (![links containsObject : s])
	//		[links addObject : s];
	//	if (![s->links containsObject : self])
	//		[s->links addObject : self];
	//}

	//gov = [Controller componentNamed : [initData objectAtIndex : count - 2] ];

	//temp = [initData objectAtIndex : count - 1];
	//types = [[Type alloc]init];
	//types->data = [[NSMutableArray alloc]initWithCapacity:[temp count] ];
	//for (i = 0; i < [temp count]; i++) {
	//	NSDictionary* dict = [temp objectAtIndex : i];
	//	TypeNode* node = [[TypeNode alloc]init];
	//	node->data = [Controller componentNamed : [dict objectForKey : @"TypeID"] ];
	//	node->probability = [[dict objectForKey : @"Probability"]floatValue];
	//	[types->data addObject : node] ;
	//}
	//[initData release] ;
}

void Solarsystem::update() {
	int i;

	if (SpaceObject::sys->viewStyle == 1)
		return;
	glDisable(GL_DEPTH_TEST);
	glColor3f(1, 1, 1);
	glLoadIdentity();
	if (SpaceObject::sys->viewStyle == 0) {
		glRotatef(-SpaceObject::sys->pitch, 1, 0, 0);
		glRotatef(-SpaceObject::sys->roll, 0, 0, 1);
		glRotatef(180 - SpaceObject::sys->angle, 0, 1, 0);
	}
	else if (SpaceObject::sys->viewStyle == 2) {
		glRotatef(SpaceObject::sys->pitch, 1, 0, 0);
		glRotatef(SpaceObject::sys->roll, 0, 0, 1);
		glRotatef(90 - SpaceObject::sys->angle, 0, 1, 0);
	}
	else if (SpaceObject::sys->viewStyle == 3) {
		glRotatef(SpaceObject::sys->deltaRot / 10, 0, 1, 0);
		glRotatef(SpaceObject::sys->deltaPitch / 10, 1, 0, 0);
		glRotatef(-SpaceObject::sys->pitch, 1, 0, 0);
		glRotatef(-SpaceObject::sys->roll, 0, 0, 1);
		glRotatef(270 - SpaceObject::sys->angle, 0, 1, 0);
	}
	for (i = 0; i < backdrops; i++) {
		float xSize = backdropWidth[i] / 4.0f;
		float ySize = backdropHeight[i] / 4.0f;
		float back = sqrt(250000 - xSize * xSize - ySize * ySize) - 50;
		glBindTexture(GL_TEXTURE_2D, backdrop[i]);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3f(-xSize, -ySize, -back);
		glTexCoord2d(1, 0);
		glVertex3f(xSize, -ySize, -back);
		glTexCoord2d(1, 1);
		glVertex3f(xSize, ySize, -back);
		glTexCoord2d(0, 1);
		glVertex3f(-xSize, ySize, -back);
		glEnd();
	}
	glEnable(GL_DEPTH_TEST);
}

Solarsystem::~Solarsystem() {
	delete types;
}