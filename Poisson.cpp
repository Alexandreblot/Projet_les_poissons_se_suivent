#include "Poisson.hpp"
#include <SDL2/SDL.h>
#include <iostream>

Poisson::Poisson(SDL_Renderer* renderer)
    : renderer(renderer), vitesse(0.0), direction(0.0), positionX(0.0), positionY(0.0) {
}

void Poisson::render() const {
    SDL_Rect destRect = {
        static_cast<int>(positionX),
        static_cast<int>(positionY),
        10,
        10
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &destRect);
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
