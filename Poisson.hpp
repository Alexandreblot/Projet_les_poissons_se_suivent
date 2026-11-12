#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include <array>
#include <vector>


class Rocher;

class Poisson {
public:
    Poisson(SDL_Renderer* renderer, double vitesse, double direction, double positionX = 0.0, double positionY = 0.0, double taille = 0.5);
    ~Poisson() = default;
    
    double vitesse;
    double direction;
    double positionX;
    double positionY;
    double taille;
    int nbVoisins = 0;

    void dessiner();
    void updatePosition();
    void suivreBanc(const std::vector<Poisson>& banc);
    void eviterRochers(const std::vector<Rocher>& rochers);
    void fuir(double sourceX, double sourceY, double rayonPeur);

private:
    SDL_Renderer* renderer;
    void handleCollisionWithWindowBounds();
};
