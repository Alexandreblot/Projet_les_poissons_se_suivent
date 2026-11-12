#pragma once

#include <SDL2/SDL.h>

class Rocher {
public:
    // texture : image partagée par tous les rochers (peut être nullptr -> disque gris)
    // rayon   : demi-taille du rocher en pixels (sert aussi à la collision)
    Rocher(SDL_Renderer* renderer, SDL_Texture* texture,
           double positionX = 0.0, double positionY = 0.0, double rayon = 30.0);
    ~Rocher() = default;

    double positionX;
    double positionY;
    double rayon;

    void dessiner();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};
