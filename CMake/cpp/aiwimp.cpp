#include "aiwimp.h"
#include "controller.h"

AIWimp::AIWimp() {
	if (float(rand()) / RAND_MAX > 0.5 && sys->planets.size() > 0) {
		curPlanet = sys->planets[rand() % sys->planets.size()];
	}
}

void AIWimp::update() {
	if (bound) {

	}
	else {
		if (curPlanet) {
			goal = DO_LAND;
			target = curPlanet;
		} else
			goal = DO_LEAVE;
		if (enemies.size() > 0) {
			float d = 150;
			if (sys->planets.size() > 0) {
				int i;
				for (auto p : sys->planets ) {
					float l = sqr(p->pos.x) + sqr(p->pos.z) + sqr(p->pos.y);
					if ( l < sqr(d)) {
						d = sqrt(l);
						curPlanet = p;
						target = p;
						goal = DO_LAND;
					}
				}
			}
			if (d > 15) {
				target = nullptr;
				goal = DO_LEAVE;
			}

		}
	}
	AI::update();
}