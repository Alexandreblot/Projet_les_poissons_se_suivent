<<<<<<< HEAD
class Poisson{
    
}


SDL_Surface* surface = IMG_Load("image.png");
SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
SDL_FreeSurface(surface); // la surface n'est plus utile une fois convertie en texture

// Position et taille de l'image à l'écran
SDL_Rect rect = {100, 100, 64, 64}; // x, y, largeur, hauteur

SDL_RenderClear(renderer);
SDL_RenderCopy(renderer, texture, NULL, &rect); // NULL = toute l'image source
SDL_RenderPresent(renderer);
=======
#include "Poisson.hpp"

#include <SDL2/SDL.h>

#include <iostream>

Poisson::Poisson(SDL_Renderer* renderer)
    : renderer(renderer), vitesse(0.0), direction(0.0), positionX(0.0), positionY(0.0) {
}

void Poisson::render() const {
    SDL_Rect destRect = { static_cast<int>(positionX), static_cast<int>(positionY), 50, 50 };
    SDL_RenderCopy(renderer, nullptr, &destRect, nullptr);
}

void Poisson::updatePosition(double X, double Y) {
    positionX += vitesse * cos(direction);
    positionY += vitesse * sin(direction);

    handleCollisionWithWindowBounds();
}

void Poisson::handleCollisionWithWindowBounds() {
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    if (positionX < 0) {
        Poisson::~Poisson();
    }

    if (positionY < 0) {
        Poisson::~Poisson();
    }
}
>>>>>>> Class-poisson
