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
