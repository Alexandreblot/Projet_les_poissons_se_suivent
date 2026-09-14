#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"




int main(){
    
    bool running = true;
    SDL_Event event;
    
    SDL_Renderer* render = launch();
    while (running) {
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        loop(render);
        
        

        SDL_Delay(16); // ~60 FPS
        
}
};