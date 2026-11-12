#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL.hpp"
#include "Poisson.hpp"
#include "Rocher.hpp"

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));

    SDL_Renderer* render = initialisation();

    IMG_Init(IMG_INIT_JPG);
    SDL_Texture* fond = IMG_LoadTexture(render, "fond_marin1.jpg");
    if (!fond) {
        std::cout << "Erreur chargement image : " << IMG_GetError() << std::endl;
    }

    SDL_Texture* imageRocher = IMG_LoadTexture(render, "rocher2.png");
    if (!imageRocher) {
        std::cout << "Erreur chargement image rocher : " << IMG_GetError() << std::endl;
    }

    int largeur, hauteur;
    SDL_GetRendererOutputSize(render, &largeur, &hauteur);

    // ---- Rochers ----
    const int nombreRochers = 30;        
    const int rayonMin = 10;            // taille minimale (demi-largeur)
    const int rayonMax = 90;            // taille maximale
    std::vector<Rocher> rochers;
    for (int i = 0; i < nombreRochers; i++) {
        double rayon = rayonMin + rand() % (rayonMax - rayonMin + 1);
        double x = rand() % largeur;
        double y = rand() % hauteur;
        rochers.emplace_back(render, imageRocher, x, y, rayon);
    }

    // ---- Poissons ----
    int nombrePoissons = 150;
    std::vector<Poisson> poissons;
    for (int i = 0; i < nombrePoissons; i++) {
        if( i%30==0 ){
            poissons.emplace_back(render, 3, rand() % 360, rand() % largeur, rand() % hauteur, 2); 
        }
        else{
            poissons.emplace_back(render, 3, rand() % 360, rand() % largeur, rand() % hauteur, 0.5);
        }
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        SDL_RenderCopy(render, fond, NULL, NULL);

        for (auto& r : rochers) {
            r.dessiner();
        }

        for (auto& p : poissons) {
            p.suivreBanc(poissons);
            p.eviterRochers(rochers);
            p.updatePosition();
            p.dessiner();
        }

        SDL_RenderPresent(render);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(imageRocher);
    SDL_DestroyTexture(fond);
    IMG_Quit();
    return 0;
}