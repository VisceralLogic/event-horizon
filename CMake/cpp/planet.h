#pragma once
#include "spaceobject.h"
#include "object3D.h"

class Planet : public SpaceObject {
public:
	//NSMutableArray* goods;			// goods sold here
	bool shipyard, mod, mission;	// available amenities
	vector<string> shipFlags;			// flags required for this ship to show up
	vector<string> modFlags;				// ...
	vector<string> missionFlags;			// ...
	string texFile;				// texture file for dynamic loading
	Object3D* model;				// used for a space station
	float inclination;				// orbital inclination from ecliptic
	string ringTex;				// path to optional ring texture file
	float ringSize;					// radius of ring
	string atmosTex;				// path to optional atmosphere texture file
	float atmosSize;				// ratio of radius of atmosphere to planetary radius
	float atmosSpeed;				// rotation of atmosphere (relative)
	float atmosRot;					// current relative angle of atmosphere
	float atmosAmbient[4];			// atmosphere ambient light
};