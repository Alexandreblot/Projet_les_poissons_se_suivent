#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"




int main(){
    int I = 0;
    while (I < 100) {
        SDL_Renderer* render = launch();

        Poisson poisson(render, 100.0, 100.0);
        poisson.affichage();
        I++;
    }

    return 0;

}