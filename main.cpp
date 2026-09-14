#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.h"




int main(){
    
    bool running = true;
    SDL_Event event;
    
    SDL_Renderer* render = launch();
    while (running) {
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        loop(render);
        //afficherImage(render, texture, rect);
        

        SDL_Delay(16); // ~60 FPS
        
}
};