#pragma once

#include <string>

using namespace std;

class EHButton {
protected:
	bool _pressed, _clicked, _mouseIn; 

public:
	int x, y;
	string text;
	void (*call)(void);
	void (*callParam)(const string& param);
	int active;
	int width;
	int (*isActive)(void);

	static void initialize();
	static void handleEvent();
	static EHButton* newButton(const string& text, int x, int y, int width, void (*call)(void), void (*callParam)(const string& param), int active, int (*isActive)(void));
	static void update();
	static void clear();
	static void removeByName(const string& name);
	static void remove(EHButton* button);
	static EHButton* buttonWithName(const string& name);
	static void setName(const string& name, const string& oldName);
	static void storeSet(const string& name);
	static void recallSet(const string& name);
	static void clearSet(const string& name);

	void draw();
	bool clicked();
	bool inBounds(int x, int y);
};