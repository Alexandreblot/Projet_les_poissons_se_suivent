#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL.hpp"
#include "Poisson.hpp"
#include "Rocher.hpp"

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));

    SDL_Renderer* render = initialisation();

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
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
    const int rayonMax = 100;           // taille maximale
    std::vector<Rocher> rochers;
    for (int i = 0; i < nombreRochers; i++) {
        double rayon = rayonMin + rand() % (rayonMax - rayonMin + 1);
        double x = rand() % largeur;
        double y = rand() % hauteur;
        rochers.emplace_back(render, imageRocher, x, y, rayon);
    }

    // ---- Obstacles créés par les clics de souris ----
    const double rayonClic = 10;        // taille de la zone qui repousse les poissons
    const Uint32 dureeClic = 100;      // durée de vie en millisecondes
    const double rayonPeur = 100;       // distance à partir de laquelle les poissons paniquent
    bool boutonEnfonce = false;         // vrai tant que le bouton de la souris est maintenu
    std::vector<Rocher> clics;          // obstacles temporaires (ils n'ont pas d'image)
    std::vector<Uint32> finClics;       // instant où chaque obstacle disparaît

    // Dessine un cercle blanc pour voir où l'on a cliqué
    auto dessinerCercle = [&](int cx, int cy, int r) {
        SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
        for (int i = 0; i < 90; i++) {
            double a = i * 2 * M_PI / 90;
            SDL_RenderDrawPoint(render, cx + static_cast<int>(r * std::cos(a)),
                                        cy + static_cast<int>(r * std::sin(a)));
        }
    };

    // ---- Poissons ----
    int nombrePoissons = 500;
    std::vector<Poisson> poissons;
    for (int i = 0; i < nombrePoissons; i++) {
        if (i % 50 == 0) {
            poissons.emplace_back(render, 3, rand() % 360, rand() % largeur, rand() % hauteur, 2);
        } else {
            poissons.emplace_back(render, 3, rand() % 360, rand() % largeur, rand() % hauteur, 0.5);
        }
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            // Clic : on crée un obstacle à l'endroit de la souris
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                clics.emplace_back(render, nullptr, event.button.x, event.button.y, rayonClic);
                finClics.push_back(SDL_GetTicks() + dureeClic);
                boutonEnfonce = true;
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                boutonEnfonce = false;
            }
        }

        SDL_RenderCopy(render, fond, NULL, NULL);

        for (auto& r : rochers) {
            r.dessiner();
        }

        // Bouton maintenu : le dernier obstacle suit la souris et ne disparaît pas
        if (boutonEnfonce && !clics.empty()) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            clics.back().positionX = mx;
            clics.back().positionY = my;
            finClics.back() = SDL_GetTicks() + dureeClic;
        }

        // On supprime les obstacles de clic expirés, puis on affiche les autres
        for (size_t i = clics.size(); i-- > 0;) {
            if (SDL_GetTicks() >= finClics[i]) {
                clics.erase(clics.begin() + i);
                finClics.erase(finClics.begin() + i);
            }
        }
        for (auto& c : clics) {
            dessinerCercle(static_cast<int>(c.positionX), static_cast<int>(c.positionY), static_cast<int>(c.rayon));
        }

        for (auto& p : poissons) {
            p.suivreBanc(poissons);
            p.eviterRochers(rochers);
            p.eviterRochers(clics);
            for (auto& c : clics) {
                p.fuir(c.positionX, c.positionY, rayonPeur);
            }
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