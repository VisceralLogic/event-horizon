#include "texture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void Texture::loadTexture(const string& file, GLuint* tex, int* width, int* height) {
	loadTexture(file.c_str(), tex, width, height);
}

void Texture::loadTexture(const char* file, GLuint* tex, int* width, int* height) {
	SDL_Surface* surface = IMG_Load(file);
	if (!surface) {
		SDL_Log("Unable to load image %s: %s", file, IMG_GetError());
		return;
	}

	GLenum format;

	if( surface->format->BytesPerPixel == 4 ) {
		if( surface->format->Rmask == 0x000000ff )
			format = GL_RGBA;
		else
			format = GL_BGRA;
	} else if( surface->format->BytesPerPixel == 3 ) {
		if( surface->format->Rmask == 0x000000ff )
			format = GL_RGB;
		else
			format = GL_BGR;
	} else {
		SDL_Log("Image is not truecolor");
		SDL_FreeSurface(surface);
		return;
	}	

	if( width )
		*width = surface->w;
	if( height )
		*height = surface->h;

	if( *tex == 0 )
		glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
}