#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "ehbutton.h"
#include "controller.h"
#include "texture.h"
#include "shader.h"

GLuint buttonTex = 0;
vector<EHButton*> buttons;
EHButton* mouseButton;
map<string, vector<EHButton*>> sets;
vector<EHButton*> toDelete;
GLShaderProgram* shader;
GLuint leftVBO, leftVAO, leftEBO, midVBO, midVAO, midEBO, rightVBO, rightVAO, rightEBO;

void EHButton::initialize() {
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
	shader->setUniformMat4("transform", glm::value_ptr(GLShaderProgram::orthoTransform));

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
	for (EHButton* button : buttons) {
		button->draw();
	}
	for(EHButton* button : toDelete) {
		delete button;
	}
	toDelete.clear();
}

void EHButton::removeByName(const string& name) {
	for( int i = 0; i < buttons.size(); i++) {
		if (buttons[i]->text == name) {
			toDelete.push_back(buttons[i]);
			buttons.erase(buttons.begin() + i);
			return;
		}
	}
}

void EHButton::remove(EHButton* button) {
	for( int i = 0; i < buttons.size(); i++) {
		if (buttons[i] == button) {
			toDelete.push_back(buttons[i]);
			buttons.erase(buttons.begin() + i);
			return;
		}
	}
}

EHButton* EHButton::buttonWithName(const string& name) {
	for( int i = 0; i < buttons.size(); i++) {
		if (buttons[i]->text == name) {
			return buttons[i];
		}
	}
	return NULL;
}

void EHButton::setName(const string& name, const string& oldName) {
	EHButton *button = buttonWithName(oldName);
	if (button) {
		button->text = name;
	}
}

void EHButton::clear() {
	for (EHButton* button : buttons) {
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

EHButton* EHButton::newButton(const string& text, int x, int y, int width, void (*call)(void), void (*callParam)(const string& param), int active, int (*isActive)(void)) {
	EHButton* button = new EHButton();
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

void EHButton::handleEvent() {
/*
	UInt32 kind = GetEventKind(event);

	if( ![buttons count] )
		return;

	switch( GetEventClass(event) ){
		case kEventClassMouse:
			if( kind == kEventMouseDown ){
				UInt16 button;
				Point p;
				GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(UInt16), NULL, &button );
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				p.v = gScreenHeight-p.v;
				if( button == kEventMouseButtonPrimary ){
					int i;
					for( i = 0; i < [buttons count]; i++ ){
						EHButton *b = [buttons objectAtIndex:i];
						if( [b inBoundsX:p.h y:p.v] ){
							b->pressed = YES;
							b->mouseIn = YES;
							mouseButton = b;
							break;
						}
					}
				}
			} else if( kind == kEventMouseUp ){
				UInt16 button;
				Point p;
				GetEventParameter( event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(UInt16), NULL, &button );
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				p.v = gScreenHeight-p.v;
				if( button == kEventMouseButtonPrimary && mouseButton ){
					if( [mouseButton inBoundsX:p.h y:p.v] ){
						if( mouseButton->pressed )
							mouseButton->clicked = YES;
					} else {
						mouseButton->pressed = NO;
					}
					mouseButton = nil;
				}
			} else if( kind == kEventMouseMoved || kind == kEventMouseDragged ){
				Point p;
				int i;
				GetEventParameter( event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &p );
				p.v = gScreenHeight-p.v;
				for( i = 0; i < [buttons count]; i++ ){
					EHButton *b = [buttons objectAtIndex:i];
					if( [b inBoundsX:p.h y:p.v] )
						b->mouseIn = YES;
					else
						b->mouseIn = NO;
				}
			}
			break;
		default:
			break;
	}
*/
}

void EHButton::draw() {
	float delta;

	if (isActive)
		active = isActive();

	if (active) {
		if (_pressed && _mouseIn)
			delta = -.5;
		else if (_mouseIn)
			delta = -.25;
		else
			delta = 0;
	}
	else
		delta = -.75;

	//glDisable(GL_DEPTH_TEST);

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

	/*if (active) {
		if (_pressed && _mouseIn)
			glColor3f(.75, .88, 1);
		else if (_mouseIn)
			glColor3f(0, 1, 1);
		else
			glColor3f(0, 1, 0);
	}
	else
		glColor3f(.2, .3, .3);
	drawString(text, (x+20.0)/gScreenWidth, (y+9.0)/gScreenHeight);*/

	//glEnable(GL_DEPTH_TEST);

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