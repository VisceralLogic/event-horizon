#pragma once
#include "spaceship.h"

class AI : public Spaceship, public enable_shared_from_this<AI> {
public:
	shared_ptr<void> target = nullptr;		// what the current goal applies to
	float startExplode = 0;					// time ship was killed
	enum {
		DO_LAND,			// land on target planet
		DO_ATTACK,			// attack target spaceship
		DO_LEAVE,			// jump to target system
		DO_LOITER,			// orbit target planet
		DO_ESCORT			// escort target ship
	} goal = DO_LOITER;		// what the ship is currently doing

	AI(shared_ptr<Spaceship> ship);
	void update();			// control this ship for one step
	void doLand();			// overrides Spacship function
	void draw();			// draw us
	void drawExplosionSize(float size, float x, float z);
	void navigate(vector<shared_ptr<Planet>>& objects);		// steer around these
	void actionLand();		// perform landing goal
	void actionAttack();
	void actionLeave();
	void actionLoiter();
	void actionEscort();
};