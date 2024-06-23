#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <chrono>
#include <ctime>
#include <iostream>

#include "SpaceObject.h"


SDL_Window* window;
SDL_GLContext context;
SDL_Rect displayBounds;
const Uint8* keyMap;
Uint8* oldKeys = nullptr;
int oldMouseX, oldMouseY;

void initGL() {
    glViewport(0, 0, displayBounds.w, displayBounds.h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0 * displayBounds.w / displayBounds.h, 0.01, 500);
    glMatrixMode(GL_MODELVIEW);

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

    // Create a window
    window = SDL_CreateWindow("SDL Application", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, displayBounds.w, displayBounds.h, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create an OpenGL context
    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        SDL_Log("Unable to create OpenGL context: %s", SDL_GetError());
        return false;
    }

    initGL();

    SpaceObject::initialize();

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
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        doUpdate();
    }
}




int main(int argc, char* args[]){
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "Current time: " << now_c << std::endl;

    if (setup())
        eventLoop();
    cleanUp();

    return 0;
}
