#pragma once

#include <SDL2/SDL.h>
#include <array>


class Poisson {
public:
    Poisson(SDL_Renderer* renderer);
    ~Poisson() = default;
    
    double vitesse;
    double direction;
    double positionX;
    double positionY;

    void render() const;

private:
    SDL_Renderer* renderer;

    void updatePosition(double X, double Y);

    void handleCollisionWithWindowBounds();

};
