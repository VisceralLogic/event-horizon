#include "aimerchant.h"
#include "controller.h"

AIMerchant::AIMerchant() {
	if (float(rand()) / RAND_MAX > 0.5) {	// 50% land, 50% leave
		float d = sys->planets.size();
		if (d > 0)
			curPlanet = sys->planets[rand() % sys->planets.size()];
	}
}

void AIMerchant::update() {
	int i;

	if (bound) {
	}
	else if (escortee) {
		goal = DO_ESCORT;
		for (auto ship : escortee->enemies) {
			if( escortee->getDistance(ship) < 25){
				target = selection = static_pointer_cast<AI>(ship);
				goal = DO_ATTACK;
			}
		}
		for (auto ship : enemies) {
			if (ship->state == DEAD) {
				enemies.erase(ship);
				continue;
			}
			if (getDistance(ship) < 50 && getDistance(escortee) < sqr(5 * size + escortee->size)) {
				target = selection = static_pointer_cast<AI>(ship);
				goal = DO_ATTACK;
			}
		}
	}
	else {
		if (curPlanet) {
			goal = DO_LAND;
			target = curPlanet;
		}
		else
			goal = DO_LEAVE;
		for (auto ship : enemies) {
			if (ship->state == DEAD) {
				enemies.erase(ship);
				continue;
			}
			if (getDistance(ship) < 50) {
				target = selection = static_pointer_cast<AI>(ship);
				goal = DO_ATTACK;
			}
		}
	}
	AI::update();
}