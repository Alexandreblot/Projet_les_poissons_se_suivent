#include "Poisson.hpp"
#include <SDL2/SDL.h>
#include <iostream>

Poisson::Poisson(SDL_Renderer* renderer, double vitesse, double direction, double positionX, double positionY)
    : renderer(renderer), vitesse(vitesse), direction(direction), positionX(positionX), positionY(positionY) {
}

void Poisson::updatePosition() {
    positionX += vitesse * cos(direction);
    positionY += vitesse * sin(direction);

    handleCollisionWithWindowBounds();
}

void Poisson::handleCollisionWithWindowBounds() {
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    const double marge = 20;

    if (positionX < -marge) {
        positionX = windowWidth + marge;
    } else if (positionX > windowWidth + marge) {
        positionX = -marge;
    }

    if (positionY < -marge) {
        positionY = windowHeight + marge;
    } else if (positionY > windowHeight + marge) {
        positionY = -marge;
    }
}

#include <cmath>

void Poisson::dessiner() {
    int cx = static_cast<int>(positionX);
    int cy = static_cast<int>(positionY);

    const double echelle = 0.1;   // 1.0 = taille d'avant, 0.5 = moitié plus petit
    const int rx = static_cast<int>(20 * echelle);        // demi-longueur du corps
    const int ry = static_cast<int>(10 * echelle);        // demi-hauteur du corps
    const int tailLen = static_cast<int>(15 * echelle);   // longueur de la queue

    // Corps
    SDL_SetRenderDrawColor(renderer, 255, 128, 0, 255);
    for (int dy = -ry; dy <= ry; dy++) {
        int half = static_cast<int>(rx * std::sqrt(1.0 - static_cast<double>(dy * dy) / (ry * ry)));
        SDL_RenderDrawLine(renderer, cx - half, cy + dy, cx + half, cy + dy);
    }

    // Queue
    for (int dx = 0; dx <= tailLen; dx++) {
        int h = dx * ry / tailLen;
        SDL_RenderDrawLine(renderer, cx - rx - dx, cy - h, cx - rx - dx, cy + h);
    }

    // Œil (un seul pixel blanc à cette taille)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, cx + rx / 2, cy - ry / 3);
}