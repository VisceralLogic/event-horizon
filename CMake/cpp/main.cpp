#include <SDL2/SDL.h>
#include <SDL_filesystem.h>
#include <SDL_image.h>
#include <glad/glad.h>
//#include <GL/glu.h>
#include <chrono>
#include <ctime>
#include <iostream>

#include "spaceobject.h"
#include "controller.h"
#include "ehbutton.h"
#include "player.h"
#include "shader.h"
#include "planet.h"
#include "background.h"
#include "solarsystem.h"

SDL_Window* window;
SDL_GLContext context;
SDL_Rect displayBounds;
const Uint8* keyMap;
Uint8* oldKeys = nullptr;
int oldMouseX, oldMouseY;
Uint64 t1, u1;

void initGL() {
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing 
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool setup() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    SDL_GetDisplayBounds(0, &displayBounds);
    gScreenWidth = 1280;// displayBounds.w;
    gScreenHeight = 800;// displayBounds.h;

    // Create a window
    window = SDL_CreateWindow("Event Horizon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenWidth, gScreenHeight, /*SDL_WINDOW_FULLSCREEN_DESKTOP |*/ SDL_WINDOW_OPENGL);
    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    // Create an OpenGL context
    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        SDL_Log("Unable to create OpenGL context: %s", SDL_GetError());
        return false;
    }

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int flags = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF);

    initGL();

    SpaceObject::initialize();
    Controller::initialize();
    GLShaderProgram::initialize();
    EHButton::initialize();
    Planet::initialize();
	Background::initialize();
    Solarsystem::initialize();

    beginPlayer();

	u1 = SDL_GetTicks64();

    return true;
}

void cleanUp(){
    // Destroy the OpenGL context
    if (context) {
        SDL_GL_DeleteContext(context);
    }

    // Destroy the window   
    if (window){
		SDL_DestroyWindow(window);
	}

	// Quit SDL
	SDL_Quit();
}

void doUpdate() {
    drawScene();
    SDL_GL_SwapWindow(window);

	if (sys != nullptr) {
        t1 = SDL_GetTicks64();
        if (t1 == u1) {
			SDL_Delay(1);
			t1 = SDL_GetTicks64();
        }
		sys->FACTOR = (t1 - u1) / 1000.0f;
		u1 = t1;
        if (sys->paused)
            sys->FACTOR = 0;
        if (sys->t == 0) {
            sys->t = 0.01f;
            sys->FACTOR = 0;
        } else 
			sys->t += sys->FACTOR;
	}
}

void eventLoop() {
    // Main loop
    bool running = true;
    while (running)
    {
        SDL_PumpEvents();
        const Uint8* keyMap = SDL_GetKeyboardState(NULL);
        if (keyMap[SDL_SCANCODE_ESCAPE])
		{
			running = false;
		}

        SDL_Event event;
        while(SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT){
				running = false;
			}
			if( eventScene != NULL ){
				eventScene(event);
			}
            EHButton::handleEvent(event);
		}

        doUpdate();
    }
}




int main(int argc, char* args[]){
    if (setup())
        eventLoop();
    cleanUp();

    return 0;
}
