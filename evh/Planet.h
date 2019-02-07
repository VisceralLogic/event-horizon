//
//  Planet.h
//  Event Horizon
//
//  Created by user on Thu Aug 08 2002.
//  Copyright (c) 2002 Paul Dorman. All rights reserved.
//

#import "SpaceObject.h"
#import "Object3D.h"

@interface Planet : SpaceObject {
	@public
	NSMutableArray *goods;			// goods sold here
	BOOL shipyard, mod, mission;	// available amenities
	NSString *shipFlags;			// flags required for this ship to show up
	NSString *modFlags;				// ...
	NSString *missionFlags;			// ...
	NSString *texFile;				// texture file for dynamic loading
	Object3D *model;				// used for a space station
	float inclination;				// orbital inclination from ecliptic
	NSString *ringTex;				// path to optional ring texture file
	float ringSize;					// radius of ring
	NSString *atmosTex;				// path to optional atmosphere texture file
	float atmosSize;				// ratio of radius of atmosphere to planetary radius
	float atmosSpeed;				// rotation of atmosphere (relative)
	float atmosRot;					// current relative angle of atmosphere
	float atmosAmbient[4];			// atmosphere ambient light
}

+ (void) registerFromDictionary:(NSDictionary *)dictionary;
- init;
- (void) finalize;
- (void) update;
- (void) drawRing;
- (void) draw;
- (void) setUpTexture;
- (void) orbit;

// useful for scripting
- (NSMutableArray *) goods;
- (NSString *) shipFlags;
- (void) setShipFlags:(NSString *)flags;
- (NSString *) modFlags;
- (void) setModFlags:(NSString *)flags;
- (NSString *) missionFlags;
- (void) setMissionFlags:(NSString *)flags;
- (void) setTextureFile:(NSString *)file;
- (void) setModel:(Object3D *)model;
- (void) setInclination:(float)f;

@end

void setUpPlanet();
void drawPlanet();
void keyPlanet( int key );
void mousePlanet( int button, int state, int x, int y );
void eventPlanet( EventRef event );
//void eventPlanet( EventRecord *event );
void nsEventPlanet( NSEvent *event );