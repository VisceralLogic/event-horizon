#include "aiagressor.h"
#include "controller.h"
#include "government.h"

void AIAgressor::update() {
	int i;
	shared_ptr<Spaceship> ship;

	//for (auto ship : enemies) {
	//	if (ship->state == DEAD) {
	//		enemies.erase(ship);
	//	}
	//}
	//if (enemies.size() == 0) {			// if no enemies, look for new
	//	if (escortee) {
	//		for (auto ship : escortee->enemies)
	//			addEnemy(ship);
	//	}
	//	else {
	//		for (auto ship : sys->ships) {
	//			if (ship.get() == this)
	//				continue;
	//			if (ship.get() == sys && sys->govRecord[gov->ID] < 0)
	//				addEnemy(ship);
	//			else if (ship.get() != sys) {
	//				if (ship->gov) {
	//					if ( gov->enemies.count(ship->gov->ID) )
	//						addEnemy(ship);
	//				}
	//				else {	// see if it doesn't have gov because it's escort


	//				}
	//			}
	//		}
	//	}
	//}

	//if (bound) {
	//}
	//else if (enemies.size() > 0 && goal != DO_ATTACK) {
	//	std::vector<shared_ptr<Spaceship>> eVector(enemies.begin(), enemies.end());
	//	target = selection = static_pointer_cast<AI>(eVector[rand() % eVector.size()]);
	//	goal = DO_ATTACK;
	//}
	//else {
	//	goal = DO_LEAVE;
	//	if (escortee)
	//		goal = DO_ESCORT;
	//}
	AI::update();
}