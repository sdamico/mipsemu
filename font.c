#include "font.h"

BitmapFont *initBitmapFont(char *file)
{
	BitmapFont *font = (BitmapFont *)malloc(sizeof(BitmapFont));
	font->font = IMG_Load(file);
	if(!(font->font))
	{
		free(font);
		return NULL;
	}
	return font;
}	

void freeBitmapFont(BitmapFont *font)
{

	if(font)
	{
		SDL_FreeSurface(font->font);
		free(font);
	}
}

int drawCharacter(BitmapFont *font, SDL_Surface *screen, char c, int x, int y)
{
	if(!font) return 0;
	short charWidth = font->font->w/95;
	short charOffset = charWidth*(c - 32);
	SDL_Rect srcRect = (SDL_Rect){charOffset,0,(short)charWidth,(short)font->font->h};
	SDL_Rect dstRect = (SDL_Rect){x,y,(short)charWidth,(short)font->font->h};
	SDL_BlitSurface(font->font, &srcRect, screen, &dstRect);
	return 1;
}

int drawString(BitmapFont *font, SDL_Surface *screen, char *str, int x, int y)
{
	if(!font) return 0;
	int len = strlen(str);
	int initialX = x;
	int charWidth = font->font->w/95;
	for(int i=0;i<len;i++)
	{
		if(str[i] == '\n')
		{
			y += font->font->h;
			x = initialX;
		}
		else
		{
			if(!drawCharacter(font,screen,str[i],x,y))
				return 0;
	
			x += charWidth;
		}
	}
	return 1;
}
