#pragma once

#include <SDL2/SDL.h>
#include <array>


class Poisson {
public:
    Poisson(SDL_Renderer* renderer, double positionX = 0.0, double positionY = 0.0);
    ~Poisson() = default;
    
    double vitesse;
    double direction;
    double positionX;
    double positionY;

    void affichage();

private:
    SDL_Renderer* renderer;

    void updatePosition(double X, double Y);

    void handleCollisionWithWindowBounds();
};
