#include "Poisson.hpp"

#include <SDL2/SDL.h>

#include <iostream>

Poisson::Poisson(SDL_Renderer* renderer, PoissonType type)
    : renderer(renderer), type(type), vitesse(0.0), direction(0.0), positionX(0.0), positionY(0.0) {
    loadTextures();
}

Poisson::~Poisson() {
    for (auto texture : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
}

Poisson::PoissonType Poisson::getType() const {
    return type;
}
void Poisson::render() const {
    SDL_Rect destRect = { static_cast<int>(positionX), static_cast<int>(positionY), 50, 50 };
    SDL_RenderCopy(renderer, getTexture(), nullptr, &destRect);
}

void Poisson::loadTextures() {
    textures[static_cast<int>(PoissonType::Rouge)] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("rouge.bmp"));
    textures[static_cast<int>(PoissonType::Vert)] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("vert.bmp"));
    textures[static_cast<int>(PoissonType::Bleu)] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("bleu.bmp"));
}

void Poisson::updatePosition(double X, double Y) {
    positionX += vitesse * cos(direction);
    positionY += vitesse * sin(direction);

    handleCollisionWithWindowBounds();
}
