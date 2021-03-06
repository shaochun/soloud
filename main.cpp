/*
SoLoud audio engine
Copyright (c) 2013 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include <stdlib.h>
#if defined(_MSC_VER)
#include "SDL.h"
#else
#include "SDL/SDL.h"
#endif
#include <math.h>

#include "soloud.h"
#include "soloud_sinewave.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"
#include "soloud_filter.h"
#include "soloud_speech.h"


SoLoud::Soloud gSoloud;
SoLoud::Sinewave gSinewave;
SoLoud::Wav gWave;
SoLoud::WavStream gWaveStream;
SoLoud::Speech gSpeech;
SoLoud::Filter gFilter;
int audiohandle;

SDL_Surface *screen;

void putpixel(int x, int y, int color)
{
	unsigned int *ptr = (unsigned int*)screen->pixels;
	int lineoffset = y * (screen->pitch / 4);
	ptr[lineoffset + x] = color;
}


void render()
{   
	// Lock surface if needed
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) 
			return;

	// Ask SDL for the time in milliseconds
	int tick = SDL_GetTicks();

	float *buf = (float*)gSoloud.mMixerData;

	//gSoloud.setPan(audiohandle, (float)sin(tick * 0.001));
	//gSoloud.setRelativePlaySpeed(audiohandle, (float)sin(tick * 0.003) * 0.1f + 1);

	int i, j;
	for (i = 0; i < 400; i++)
	{
		int v = (int)floor(buf[i*2]*127+128);
		for (j = 0; j < 256; j++)
		{
			int c = 0;
			if (j < 128 && v < 128 && j > v)
				c = 0xff0000;
			if (j > 127 && v > 127 && j < v)
				c = 0xff0000;

			putpixel(i, j, c);
		}
	}

	// Unlock if needed
	if (SDL_MUSTLOCK(screen)) 
		SDL_UnlockSurface(screen);

	// Tell SDL to update the whole screen
	SDL_UpdateRect(screen, 0, 0, 400, 256);    
}


// Entry point
int main(int argc, char *argv[])
{
	gWaveStream.load("jingle.ogg");
	gWaveStream.setLooping(1);
	int i;

	gSpeech.setText("    1 2 3     1 2 3     testing testing     welcome to so loud");
	gFilter.setSource(&gSpeech);
	
	// Initialize SDL's subsystems - in this case, only video.
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	SoLoud::sdl_init(&gSoloud);

	// Register SDL_Quit to be called at exit; makes sure things are
	// cleaned up when we quit.
	atexit(SDL_Quit);	

	// Attempt to create a 640x480 window with 32bit pixels.
	screen = SDL_SetVideoMode(400, 256, 32, SDL_SWSURFACE);

	// If we fail, return error.
	if ( screen == NULL ) 
	{
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}

	int songhandle = gSoloud.play(gWaveStream,1,0);
	gSoloud.fadeVolume(songhandle, 0, 1, 5);
	gSoloud.setGlobalVolume(2);
	int voicehandle = gSoloud.play(gFilter);
	gSoloud.fadePan(voicehandle, -1, 1, 5);

	// Main loop: loop forever.
	while (1)
	{
		// Render stuff
		render();

		// Poll for events, and handle the ones we care about.
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				// If escape is pressed, return (and thus, quit)
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					SoLoud::sdl_deinit(&gSoloud);
					return 0;
				}
				break;
			case SDL_QUIT:
				SoLoud::sdl_deinit(&gSoloud);
				return(0);
			}
		}
	}
	return 0;
}