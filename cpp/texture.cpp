#include "texture.h"
#include <SDL.h>
#include <SDL_image.h>

void Texture::loadTexture(const char* file, GLuint* tex, int* width, int* height) {
	SDL_Surface* surface = IMG_Load(file);
	if (!surface) {
		SDL_Log("Unable to load image %s: %s", file, IMG_GetError());
		return;
	}

	if( *width )
		*width = surface->w;
	if( *height )
		*height = surface->h;

	if( *tex == 0 )
		glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
}