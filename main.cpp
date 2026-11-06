#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));

    SDL_Renderer* render = initialisation();
    int nombrePoissons = 500;
    std::vector<Poisson> poissons;
    for (int i = 0; i < 5; i++) {
        poissons.emplace_back(render, rand() % 100 + 1, rand() % 360, rand() % 640, rand() % 480);
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        for (auto& p : poissons) {
            p.suivreBanc(poissons);
            p.updatePosition();
            p.dessiner();
        }

        SDL_RenderPresent(render);
        SDL_Delay(16);
    }

    return 0;
}