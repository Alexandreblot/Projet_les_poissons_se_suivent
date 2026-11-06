#include <SDL2/SDL.h>
#include <iostream>
#include "SDL.hpp"

SDL_Renderer* initialisation() {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(1900, 1080, SDL_WINDOW_SHOWN, &window, &renderer);

    return renderer;
}

void loop(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawPoint(renderer, 320, 240);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void afficherImage(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect rect) {
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void deplacerImage(int &x, int &y, int vitesse) {
    const Uint8* keys = SDL_GetKeyboardState(NULL); // Récupère les touches du clavier entrée
    if (keys[SDL_SCANCODE_RIGHT]) x += vitesse;
    if (keys[SDL_SCANCODE_LEFT])  x -= vitesse;
    if (keys[SDL_SCANCODE_UP])    y -= vitesse;
    if (keys[SDL_SCANCODE_DOWN])  y += vitesse;
}