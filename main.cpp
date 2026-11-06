#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"


int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));

    SDL_Renderer* render = initialisation();

    IMG_Init(IMG_INIT_JPG);
    SDL_Texture* fond = IMG_LoadTexture(render, "fond_marin.jpg");
    if (!fond) {
        std::cout << "Erreur chargement image : " << IMG_GetError() << std::endl;
    }

    int nombrePoissons = 100;
    std::vector<Poisson> poissons;
    for (int i = 0; i < nombrePoissons; i++) {
        poissons.emplace_back(render, 3, rand() % 360, rand() % 640, rand() % 480);
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        SDL_RenderCopy(render, fond, NULL, NULL);

        for (auto& p : poissons) {
            p.suivreBanc(poissons);
            p.updatePosition();
            p.dessiner();
        }

        SDL_RenderPresent(render);
        SDL_Delay(16);
    }


    SDL_DestroyTexture(fond);
    IMG_Quit();
    return 0;
}