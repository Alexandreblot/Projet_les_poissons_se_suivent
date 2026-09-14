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

    PoissonType getType() const;

    Poisson(SDL_Renderer* renderer, PoissonType type);
    ~Poisson();

    SDL_Texture* getTexture() const {
        return textures[static_cast<int>(type)];
    }

    double vitesse;
    double direction;
    double positionX;
    double positionY;

private:
    SDL_Renderer* renderer;
    PoissonType type;
    std::array<SDL_Texture*, 3> textures;

    void loadTextures();

    void updatePosition();

    void render() const;

    void handleCollisionWithWindowBounds();

};
