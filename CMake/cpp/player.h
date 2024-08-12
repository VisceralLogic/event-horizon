#pragma once
#include "controller.h"

void drawPlayerScreen();
void eventPlayer(SDL_Event &event);
void setUpPlayerTex();
void beginPlayer();
void endPlayer(bool newPlayer);
//void eventPlayer(EventRef event);
//void nsEventPlayer(NSEvent* event);
void playerKeyFunc(unsigned char key);
void playerMouse();