#include <SDL/SDL.h>
#include "mips.h"
#include "font.h"


int main(int argc, char **argv)
{
	// Initialize our CPU with 8kBytes of memory
	// Set PC to address 4096
	// Set 0xFF for graphics memory mapping, 0xFD for arrow keys, and 0xFC for mouse
	MIPS *m = initMIPS(8192, 4096,0xFF,0xFD,0xFC);

	// print the usage info for the program if we don't enter proper arguments
	if(argc != 2) printf("Usage: %s [code.bin]\n",argv[0]);

	//int y = 0;

	// Load the program (starting at address 4096 - what we set PC to)
	if(loadProgram(m, argv[1], 4096))
	{
		printf("Program %s loaded.\n",argv[1]);
		SDL_Surface *screen;

		SDL_Init(SDL_INIT_VIDEO);

		if (!(screen = SDL_SetVideoMode(800, 640, 24, SDL_SWSURFACE)))
    		{
    	    		SDL_Quit();
        		return 0;
    		}
	
		char titleString[80];
		sprintf(titleString,"MIPSEMU: %s", argv[1]);
		SDL_WM_SetCaption(titleString,"MIPSEMU");

		IMG_Init(IMG_INIT_PNG);
		BitmapFont *font = initBitmapFont("bitmapfont.png");

		SDL_Event event;
		Uint8 *keys = SDL_GetKeyState(NULL);
		long clock = 0;

		long lastTicks = 0;
		long ticks;
		char buffer[80];
		double speedMhz = 50;
		SDL_Rect statusRegion;
		statusRegion.x = 0;
		statusRegion.y = 600;
		statusRegion.w = 800;
		statusRegion.h = 60;
		
		while(emulate(m) && !keys[SDLK_ESCAPE])
		{
			// Update memory mapping for mouse and keyboard input
			// Only ever 10,000 clock cycles so we don't slow down
			// the CPU
			if(!(clock % 10000))
			{
				SDL_PollEvent(&event);
				keys = SDL_GetKeyState(NULL);
				SDL_GetMouseState(&m->mouseX,&m->mouseY);
				// Update arrow keys - LSB bits 0-3
				m->arrowKeys = keys[SDLK_UP] + (keys[SDLK_DOWN]<<1) + (keys[SDLK_LEFT]<<2) + (keys[SDLK_RIGHT]<<3);
							
				if(m->mouseY < 0) m->mouseY = 0;
				else if(m->mouseY > 600) m->mouseY = 600;
				if(m->mouseX < 0) m->mouseX = 0;
				else if(m->mouseX > 800) m->mouseX = 800;
			}
			if(!(clock % 1000000))
			{
				SDL_FillRect(screen, &statusRegion, 0);
				ticks = SDL_GetTicks();
				speedMhz = 1000.0f/((double)(ticks-lastTicks));
				sprintf(buffer,"%0.2f MHz, PC=0x%.8x",speedMhz,byteSwap(m->pc));
				lastTicks = ticks;
				drawString(font,screen,buffer,40,600);
				SDL_UpdateRect(screen,0,600,800,32);
				
			}
			if(m->mmapGFX)
			{
				m->mmapGFX = 0;
				unsigned int pixel = *(int *)(&(((char *)m->memory)[255]));
				int x = (pixel >> 16) & 0xFF;
				int y = (pixel >> 24) & 0xFF;
				int color = ((pixel >> 8) & 0xFF);

				if ( SDL_MUSTLOCK(screen) ) {
  					if ( SDL_LockSurface(screen) < 0 ) {
						fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
						return 0;
 					}
				}
	
				char *sColor;
				for(int i=0;i<20;i++)
					for(int j=0;j<20;j++)
					{
						if(x < 40 && y < 30)
						{
							sColor = ((char *)screen->pixels+(y*20+i)*screen->pitch+(x*20+j)*screen->format->BytesPerPixel);
							sColor[0] = (color & 3)*85;
							sColor[1] = ((color >> 2) & 3)*85;
							sColor[2] = ((color >> 4) & 3)*85;
						}
					}
				if ( SDL_MUSTLOCK(screen) ) {
					SDL_UnlockSurface(screen);
				}

				SDL_UpdateRect(screen,x*20,y*20,20,20);
				
				//printf("GFX %i %i %i\n",x,y,color);
			}
			clock++;
			asm("nop");
		}

		freeBitmapFont(font);
		IMG_Quit();
		SDL_Quit();

	}
	freeMIPS(m);
	return 1;
}
