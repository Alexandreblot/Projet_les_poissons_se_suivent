#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"




int main(){
    int I = 0;
    SDL_Renderer* render = initialisation();
    while (I < 2) {
        Poisson poisson(render,  rand() % 640, rand() % 480);
        poisson.affichage();
        I++;
        SDL_Delay(1000);
    }

    return 0;
}