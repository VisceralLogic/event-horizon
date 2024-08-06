#pragma once
#include <string>

using namespace std;

class EHMenuItem {
public:
	string name;			// name to display
	//keyCode hotkey;			// key to respond to
	enum {
		ACTIVE,
		SELECTED
	} state;				// current display state
	string menu;			// menu for submenu items
	int width;				// menu width
	//id target;				// object to perform action on
	//SEL action;				// action to perform
	//id object;				// objec to pass in action
	bool objectPresent;		// if there is an object to pass
};