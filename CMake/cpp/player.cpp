#include <filesystem>
#include <iostream>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "player.h"
#include "texture.h"
#include "ehbutton.h"
#include "shader.h"

extern void DrawGLScene();

void loadPlayerNames();
void drawButton(char* title, int x, int y, int width, int height, bool active);
int validPlayer();
void playerDelete();
void playerEnter();
void playerQuit();
void playerNew();
void playerName(const string& name);
void playerCancel();

int playerIndex = 0;
GLuint playerTex, splitLogoTex;
int numPlayers = 0;
bool selectPlayer = true;
float t = 0;
int gScreenWidth, gScreenHeight;
string name;
Uint64 u, myT;
vector<string> players;
float pluginLoading = 0;
GLShaderProgram* logoShader;
GLuint logoVBO, logoVAO, logoEBO, splitVAO, splitVBO, splitEBO;

void beginPlayer() {
	drawScene = drawPlayerScreen;
	eventScene = eventPlayer;
	Texture::loadTexture(Controller::basePath + "Images/LogoLarge.png", &playerTex, NULL, NULL);
	Texture::loadTexture(Controller::basePath + "Images/Split.png", &splitLogoTex, NULL, NULL);
	u = SDL_GetTicks64();
	SDL_ShowCursor(SDL_ENABLE);
	EHButton::newButton("Enter", gScreenWidth - 164, 50, 128, playerEnter, NULL, false, validPlayer);
	EHButton::newButton("New Player...", gScreenWidth / 2 - 200, gScreenHeight - 288, 200, playerNew, NULL, true, NULL);	
	EHButton::newButton("Quit", 164, 50, 128, playerQuit, NULL, true, NULL);
	EHButton::newButton("Delete", gScreenWidth - 528, 50, 128, playerDelete, NULL, false, validPlayer);
	loadPlayerNames();

	float logoVertices[] = {
		gScreenWidth / 2 - 256, gScreenHeight / 2 - 256, 0, 0, 1,
		gScreenWidth / 2 + 256, gScreenHeight / 2 - 256, 0, 1, 1,
		gScreenWidth / 2 + 256, gScreenHeight / 2 + 256, 0, 1, 0,
		gScreenWidth / 2 - 256, gScreenHeight / 2 + 256, 0, 0, 0
	};
	float splitVertices[] = {
		(gScreenWidth - 733)/2, gScreenHeight - 256, 0, 0, 0.5,
		gScreenWidth/2 + 145, gScreenHeight - 256, 0, 1, 0.5,
		gScreenWidth/2 + 145, gScreenHeight, 0, 1, 0,
		(gScreenWidth - 733) / 2, gScreenHeight, 0, 0, 0,
		gScreenWidth/2 + 145, gScreenHeight - 256, 0, 0, 1,
		(gScreenWidth + 733) / 2, gScreenHeight - 256, 0, 0.43164, 1,
		(gScreenWidth + 733) / 2, gScreenHeight, 0, 0.43164, 0.5,
		gScreenWidth/2 + 145, gScreenHeight, 0, 0, 0.5
	};
	GLuint logoIndices[] = {
		0, 1, 2,
		2, 3, 0
	};
	GLuint splitIndices[] = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4
	};

	string vertexShader = "#version 330 core\n"
		"layout(location = 0) in vec3 pos;\n"
		"layout(location = 1) in vec2 tex;\n"
		"out vec2 TexCoord;\n"
		"uniform mat4 transform;\n"
		"void main() {\n"
		"	gl_Position = transform * vec4(pos, 1.0);\n"
		"	TexCoord = tex;\n"
		"}\n";

	string fragmentShader = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D tex;\n"
		"uniform float brightness;\n"
		"void main() {\n"
		"	FragColor = texture(tex, TexCoord) * vec4(brightness, brightness, brightness, 1.0);\n"
		"}\n";


	logoShader = new GLShaderProgram(vertexShader, fragmentShader);
	logoShader->use();
	logoShader->setUniformMat4("transform", glm::value_ptr(GLShaderProgram::orthoTransform));

	glGenVertexArrays(1, &logoVAO);
	glBindVertexArray(logoVAO);

	glGenBuffers(1, &logoVBO);
	glBindBuffer(GL_ARRAY_BUFFER, logoVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(logoVertices), logoVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &logoEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, logoEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(logoIndices), logoIndices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &splitVAO);
	glBindVertexArray(splitVAO);

	glGenBuffers(1, &splitVBO);
	glBindBuffer(GL_ARRAY_BUFFER, splitVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(splitVertices), splitVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &splitEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, splitEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(splitIndices), splitIndices, GL_STATIC_DRAW);
}

void endPlayer(bool newPlayer) {
	EHButton::clearSet("Player");
	EHButton::clear();
	SDL_ShowCursor(SDL_DISABLE);
	if (!sys) {
		sys = new Controller(name, newPlayer);
		sys->screenWidth = gScreenWidth;
		sys->screenHeight = gScreenHeight;
	}
	else {
		sys->name = name;
		//sys->load();
	}
	//if (sys->floatVal[MOUSE] == 1.0f)
	//	CGAssociateMouseAndMouseCursorPosition(NO);
	drawScene = drawGLScene;
	//eventScene = NULL;

	delete logoShader;
	glDeleteTextures(1, &playerTex);
	glDeleteTextures(1, &splitLogoTex);
	glDeleteBuffers(1, &logoVBO);
	glDeleteBuffers(1, &logoVAO);
	glDeleteBuffers(1, &logoEBO);
	glDeleteBuffers(1, &splitVBO);
	glDeleteBuffers(1, &splitVAO);
	glDeleteBuffers(1, &splitEBO);
}

void drawPlayerScreen() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myT = SDL_GetTicks64();
	if (t == 0)
		t = 0.01;
	else
		t += (myT - u) / 1E3;
	u = myT;

	logoShader->use();

	if (pluginLoading != 0 || t >= 10) {
		logoShader->setUniformf("brightness", 1.0f);
		glBindTexture(GL_TEXTURE_2D, splitLogoTex);
		glBindVertexArray(splitVAO);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	if (pluginLoading != 0) {
		float color[3] = { 0, 1, 0 };
		Controller::drawString("Loading...", (gScreenWidth / 2.0 - 128) / gScreenWidth, 250.0 / gScreenHeight, color);
		glColor3f(1, 0, 0);
		glBegin(GL_LINE_STRIP);
		glVertex2i(gScreenWidth / 2 - 129, 199);
		glVertex2i(gScreenWidth / 2 + 129, 199);
		glVertex2i(gScreenWidth / 2 + 129, 217);
		glVertex2i(gScreenWidth / 2 - 129, 217);
		glVertex2i(gScreenWidth / 2 - 129, 199);
		glEnd();
		glBegin(GL_QUADS);
		glColor3f(0, 0, 1);
		glVertex2i(gScreenWidth / 2 - 128, 216);
		glVertex2i(gScreenWidth / 2 - 128, 200);
		glColor3f(0, 0, 0);
		glVertex2f(gScreenWidth / 2 - 128 + 256 * pluginLoading, 200);
		glVertex2f(gScreenWidth / 2 - 128 + 256 * pluginLoading, 216);
		glEnd();
	}
	else if (t < 10) {
		logoShader->setUniformf("brightness", t < 5 ? t / 5 : 1.0f);
		glBindTexture(GL_TEXTURE_2D, playerTex);
		glBindVertexArray(logoVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	else {		
		EHButton::update();

		if (selectPlayer) {		// draw selection screen
			int i;

			for (i = 0; i < numPlayers; i++) {
				if (playerIndex == i + 1) {
					float color[3] = { .5 + .5 * sin(6 * t), 1, 0 };
					Controller::drawString(">", (gScreenWidth / 2.0 - 196) / gScreenWidth, (gScreenHeight - 278 - 40 * (i + 1.0)) / gScreenHeight, color);
				}
			}
		}
		else {	// creating new player
			float color[3] = { 1, 1, 0 };
			Controller::drawString("Enter your name:", (gScreenWidth / 2.0 - 200) / gScreenWidth, (gScreenHeight - 288.0) / gScreenHeight, color);
			color[0] = 0;
			Controller::drawString(name, (gScreenWidth / 2.0 - 76) / gScreenWidth, (gScreenHeight - 288.0) / gScreenHeight, color);

			// TO DO: get raster pos, if too long, stop entry
			/*if (sin(8 * t) > 0.5)
				DrawCStringGL("|", font);*/
		}
	}
}

void eventPlayer(SDL_Event &event) {
	if (event.type == SDL_MOUSEBUTTONDOWN ){
		if( t < 10 )
			t = 10;
	}
	else if (event.type == SDL_MOUSEMOTION) {

	}
}

void loadPlayerNames() {

}

int validPlayer() {
	if ((selectPlayer && playerIndex > 0 && playerIndex <= numPlayers) || name.size() > 0)
		return 1;
	return 0;
}

void playerDelete() {

}

void playerEnter() {
	if (selectPlayer) {
		if (playerIndex > 0)
			endPlayer(false);
	}
	else {
		if (name.size() > 0)
			return;
		endPlayer(true);
		//setUpPrefs();
	}
}

void playerQuit() {
	exit(0);
}

void playerNew() {

}