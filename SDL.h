#ifndef SDL_H
#define SDL_H

#include <SDL2/SDL.h>

//fonctions qu'on voudra utiliser dans main.cpp
SDL_Renderer* launch();
void loop(SDL_Renderer* renderer);
void afficherImage(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect rect);
void deplacerImage(int &x, int &y, int vitesse);

    
#endif

