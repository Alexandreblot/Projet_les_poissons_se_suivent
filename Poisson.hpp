#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include <array>


class Poisson {
public:
    Poisson(SDL_Renderer* renderer, double vitesse, double direction, double positionX = 0.0, double positionY = 0.0);
    ~Poisson() = default;
    
    double vitesse;
    double direction;
    double positionX;
    double positionY;

    void dessiner();
    void updatePosition();
    void suivreBanc(const std::vector<Poisson>& banc);

private:
    SDL_Renderer* renderer;

    void handleCollisionWithWindowBounds();
};
