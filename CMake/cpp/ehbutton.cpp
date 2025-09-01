#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "ehbutton.h"
#include "controller.h"
#include "texture.h"
#include "shader.h"

GLuint EHButton::buttonTex;
vector<shared_ptr<EHButton>> EHButton::buttons;
shared_ptr<EHButton> EHButton::mouseButton;
map<string, vector<shared_ptr<EHButton>>> EHButton::sets;
vector<shared_ptr<EHButton>> EHButton::toDelete;

void EHButton::initialize() {
	buttonTex = 0;
	int tempWidth = 0, tempHeight = 0;
	Texture::loadTexture(Controller::basePath + "Images/Button.png", &buttonTex, &tempWidth, &tempHeight);
}

void EHButton::update() {
	for (shared_ptr<EHButton> button : buttons) {
		button->draw();
	}
	for(shared_ptr<EHButton> button : toDelete) {
		for (auto it = buttons.begin(); it != buttons.end(); ++it) {
			if (button == *it) {
				buttons.erase(it, it + 1);
				break;
			}
		}
	}
	toDelete.clear();
}

void EHButton::removeByName(const string& name) {
	for( shared_ptr<EHButton> button : buttons ) {
		if (button->text == name) {
			toDelete.push_back(button);
			return;
		}
	}
}

void EHButton::remove(shared_ptr<EHButton> button) {
	for( shared_ptr<EHButton> b : buttons ) {
		if (b == button) {
			toDelete.push_back(button);
			return;
		}
	}
}

shared_ptr<EHButton> EHButton::buttonWithName(const string& name) {
	for( int i = 0; i < buttons.size(); i++) {
		if (buttons[i]->text == name) {
			return buttons[i];
		}
	}
	return NULL;
}

void EHButton::setName(const string& name, const string& oldName) {
	shared_ptr<EHButton> button = buttonWithName(oldName);
	if (button) {
		button->text = name;
	}
}

void EHButton::clear() {
	for (shared_ptr<EHButton> button : buttons) {
		toDelete.push_back(button);
	}
	buttons.clear();
}

void EHButton::storeSet(const string& name) {
	sets[name] = buttons;
}

void EHButton::recallSet(const string& name) {
	clear();
	buttons = sets[name];
}

void EHButton::clearSet(const string& name) {
	sets.erase(name);
}

shared_ptr<EHButton> EHButton::newButton(const string& text, int x, int y, int width, void (*call)(void), void (*callParam)(const string& param), int active, int (*isActive)(void)) {
	shared_ptr<EHButton> button(new EHButton());
	button->text = text;
	button->x = x;
	button->y = y;
	button->width = width;
	button->call = call;
	button->callParam = callParam;
	button->active = active;
	button->isActive = isActive;
	buttons.push_back(button);
	return button;
}

void EHButton::handleEvent(SDL_Event &event) {
	if (event.type == SDL_MOUSEMOTION) {
				int x, y;
		SDL_GetMouseState(&x, &y);
		y = gScreenHeight - y;
		for( shared_ptr<EHButton> b : buttons ){
			if( b->inBounds(x, y) )
				b->_mouseIn = true;
			else
				b->_mouseIn = false;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		y = gScreenHeight - y;
		// check if left button
		if( event.button.button != SDL_BUTTON_LEFT )
			return;
		for( shared_ptr<EHButton> b : buttons ){
			if( b->inBounds(x, y) ){
				b->_pressed = true;
				mouseButton = b;
				break;
			}
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		y = gScreenHeight - y;
		if( mouseButton.get() && event.button.button == SDL_BUTTON_LEFT) {
			if (mouseButton->inBounds(x, y)) {
				mouseButton->_clicked = true;
			}
			else {
				mouseButton->_pressed = false;
			}
			mouseButton = nullptr;
		}
	}
}

void EHButton::draw() {
	float delta;

	if (isActive)
		active = isActive();

	if (active) {
		if (_pressed && _mouseIn)
			delta = -.25;
		else if (_mouseIn)
			delta = -.5;
		else
			delta = -.75;
	}
	else
		delta = 0;

	glDisable(GL_DEPTH_TEST);

	GLShaderProgram::drawRectangle(x, y, 32, 32, 0, -.25f+delta, .25f, 0+delta, buttonTex);
	GLShaderProgram::drawRectangle(x+32, y , width-64, 32, .25f, -.25f+delta, .75f, 0+delta, buttonTex);
	GLShaderProgram::drawRectangle(x+width-32, y, 32, 32, .75f, -.25f+delta, 1, 0+delta, buttonTex);

	float* color;
	float pressedAndMouse[3] = {0.75f, 0.88f, 1};
	float mouseIn[3] = {0, 1, 1};
	float mouseOut[3] = { 0, 1, 0 };
	float inactive[3] = { .2f, .3f, .3f };
	if (active) {
		if (_pressed && _mouseIn)
			color = pressedAndMouse;
		else if (_mouseIn)
			color = mouseIn;
		else
			color = mouseOut;
	}
	else
		color = inactive;

	Controller::drawString(text, x+20.0f, y+9.0f, color);

	glEnable(GL_DEPTH_TEST);

	if (_pressed && _clicked) {   // fresh click
		_pressed = false;
		if (call) {
			call();
		}
		if (callParam)
			callParam(text);
	}
	else if (_clicked) {
		_clicked = false;
	}
}

bool EHButton::clicked() {
	return _clicked;
}

bool EHButton::inBounds(int x, int y) {
	return (x >= this->x && x <= this->x + width && y >= this->y && y <= this->y + 32);
}