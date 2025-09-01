#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <SDL2/SDL.h>
#include <glad/glad.h>

class GLShaderProgram;

using namespace std;

class EHButton {
protected:
	bool _pressed = false, _clicked = false, _mouseIn = false; 

	static vector<shared_ptr<EHButton>> buttons;
	static shared_ptr<EHButton> mouseButton;
	static map<string, vector<shared_ptr<EHButton>>> sets;
	static vector<shared_ptr<EHButton>> toDelete;
	static GLuint buttonTex;

public:
	int x, y;
	string text;
	void (*call)(void);
	void (*callParam)(const string& param);
	int active;
	int width;
	int (*isActive)(void);

	static void initialize();
	static void handleEvent(SDL_Event &event);
	static shared_ptr<EHButton> newButton(const string& text, int x, int y, int width, void (*call)(void), void (*callParam)(const string& param), int active, int (*isActive)(void));
	static void update();
	static void clear();
	static void removeByName(const string& name);
	static void remove(shared_ptr<EHButton> button);
	static shared_ptr<EHButton> buttonWithName(const string& name);
	static void setName(const string& name, const string& oldName);
	static void storeSet(const string& name);
	static void recallSet(const string& name);
	static void clearSet(const string& name);

	void draw();
	bool clicked();
	bool inBounds(int x, int y);
};