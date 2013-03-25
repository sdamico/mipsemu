#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct 
{
	SDL_Surface *font;
}BitmapFont;

BitmapFont *initBitmapFont(char *file);

void freeBitmapFont(BitmapFont *font);

int drawCharacter(BitmapFont *font, SDL_Surface *screen, char c, int x, int y);

int drawString(BitmapFont *font, SDL_Surface *screen, char *str, int x, int y);

