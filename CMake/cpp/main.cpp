#include <SDL2/SDL.h>
#include <SDL_filesystem.h>
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

SDL_Window* window;
SDL_GLContext context;
SDL_Rect displayBounds;
const Uint8* keyMap;
Uint8* oldKeys = nullptr;
int oldMouseX, oldMouseY;

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
    gScreenWidth = displayBounds.w;
    gScreenHeight = displayBounds.h;

    // Create a window
    window = SDL_CreateWindow("Event Horizon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenWidth, gScreenHeight, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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

    initGL();

    SpaceObject::initialize();
    Controller::initialize();
    GLShaderProgram::initialize();
    EHButton::initialize();


    beginPlayer();

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
