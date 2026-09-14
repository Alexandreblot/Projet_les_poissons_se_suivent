#pragma once

#include <SDL2/SDL.h>
#include <array>


class Poisson {
public:
    enum class PoissonType {
        Rouge,
        Vert,
        Bleu
    };

    Poisson(SDL_Renderer* renderer, PoissonType type);
    ~Poisson();

    SDL_Texture* getTexture() const {
        return textures[static_cast<int>(type)];
    }
private:
    SDL_Renderer* renderer;
    PoissonType type;
    std::array<SDL_Texture*, 3> textures;
    double vitesse;
    double direction;
    double positionX;
    double positionY;

    void loadTextures();

};
