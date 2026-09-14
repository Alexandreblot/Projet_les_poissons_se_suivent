#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"




int main(){
    bool running = true;
    SDL_Event event;

    SDL_Renderer* render = launch();

    Poisson poisson(render);
    poisson.positionX = 100.0;
    poisson.positionY = 100.0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        poisson.render();

        SDL_RenderPresent(render);
        SDL_Delay(16);
    }

    return 0;
}