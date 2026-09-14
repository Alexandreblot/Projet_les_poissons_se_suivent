#include <iostream>
#include <SDL2/SDL.h>
#include "SDL.hpp"
#include "Poisson.hpp"




int main(){
    SDL_Renderer* render = launch();

    Poisson poisson(render, 100.0, 100.0);
    poisson.affichage();

    return 0;
}