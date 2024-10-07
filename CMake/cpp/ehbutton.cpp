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
GLShaderProgram* EHButton::shader;
GLuint EHButton::leftVBO, EHButton::leftVAO, EHButton::leftEBO, EHButton::midVBO, EHButton::midVAO, EHButton::midEBO, EHButton::rightVBO, EHButton::rightVAO, EHButton::rightEBO;

void EHButton::initialize() {
	buttonTex = 0;
	int tempWidth = 0, tempHeight = 0;
	Texture::loadTexture(Controller::basePath + "Images/Button.png", &buttonTex, &tempWidth, &tempHeight);

	string vertexShader = "#version 330 core\n"
		"layout(location = 0) in vec3 pos;\n"
		"layout(location = 1) in vec2 tex;\n"
		"out vec2 TexCoord;\n"
		"uniform mat4 transform;\n"
		"uniform float width;\n"
		"uniform float xOffset;\n"
		"uniform float yOffset;\n"
		"uniform float delta;\n"
		"void main() {\n"
		"	gl_Position = transform * vec4(pos.x * width + xOffset, pos.y + yOffset, pos.z, 1.0);\n"
		"	TexCoord = vec2(tex.x, tex.y+delta);\n"
		"}\n";

	string fragmentShader = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D tex;\n"
		"void main() {\n"
		"	FragColor = texture(tex, TexCoord);\n"
		"}\n";

	shader = new GLShaderProgram(vertexShader, fragmentShader);
	shader->use();
	shader->setUniformMat4("transform", GLShaderProgram::orthoTransform);

	float leftVertices[] = {
		0, 0, 0, 0, 0,
		1, 0, 0, .25, 0,
		1, 32, 0, .25, -.25,
		0, 32, 0, 0, -.25
	};

	float midVertices[] = {
		0, 0, 0, .25, 0,
		1, 0, 0, .75, 0,
		1, 32, 0, .75, -.25,
		0, 32, 0, .25, -.25
	};

	float rightVertices[] = {
		0, 0, 0, .75, 0,
		1, 0, 0, 1, 0,
		1, 32, 0, 1, -.25,
		0, 32, 0, .75, -.25
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &leftVAO);
	glBindVertexArray(leftVAO);

	glGenBuffers(1, &leftVBO);
	glBindBuffer(GL_ARRAY_BUFFER, leftVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leftVertices), leftVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &leftEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leftEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &midVAO);
	glBindVertexArray(midVAO);

	glGenBuffers(1, &midVBO);
	glBindBuffer(GL_ARRAY_BUFFER, midVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(midVertices), midVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &midEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, midEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &rightVAO);
	glBindVertexArray(rightVAO);

	glGenBuffers(1, &rightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, rightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rightVertices), rightVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &rightEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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

	shader->use();
	glBindTexture(GL_TEXTURE_2D, buttonTex);
	shader->setUniformf("width", 32);
	shader->setUniformf("xOffset", x);
	shader->setUniformf("yOffset", y);
	shader->setUniformf("delta", delta);
	glBindVertexArray(leftVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	shader->setUniformf("xOffset", x + 32);
	shader->setUniformf("width", width - 64);
	glBindVertexArray(midVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	shader->setUniformf("xOffset", x + width - 32);
	shader->setUniformf("width", 32);
	glBindVertexArray(rightVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	float* color;
	float pressedAndMouse[3] = {0.75, 0.88, 1};
	float mouseIn[3] = {0, 1, 1};
	float mouseOut[3] = { 0, 1, 0 };
	float inactive[3] = { .2, .3, .3 };
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

	Controller::drawString(text, x+20.0, y+9.0, color);

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