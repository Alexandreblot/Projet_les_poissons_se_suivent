#include "Poisson.hpp"
#include <SDL2/SDL.h>
#include <iostream>

Poisson::Poisson(SDL_Renderer* renderer, double positionX, double positionY)
    : renderer(renderer), vitesse(0.0), direction(0.0), positionX(positionX), positionY(positionY) {
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

void Poisson::affichage() {
    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect rect = {
            static_cast<int>(positionX),
            static_cast<int>(positionY),
            10,
            10
        };

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}