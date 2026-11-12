#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <algorithm>
#include <filesystem>
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

    // ---- Sons joués quand on pose un rocher (tous les .wav du dossier "sons") ----
    const std::string dossierSons = "sons";
    SDL_AudioDeviceID audio = 0;
    std::vector<std::vector<Uint8>> sons;   // chaque son, converti au format du périphérique audio

    SDL_AudioSpec voulu{}, obtenu{};
    voulu.freq = 44100;
    voulu.format = AUDIO_S16SYS;
    voulu.channels = 2;
    voulu.samples = 1024;
    audio = SDL_OpenAudioDevice(NULL, 0, &voulu, &obtenu, 0);
    if (audio == 0) {
        std::cout << "Erreur ouverture audio : " << SDL_GetError() << std::endl;
    } else {
        SDL_PauseAudioDevice(audio, 0);   // démarre la lecture

        namespace fs = std::filesystem;
        if (fs::is_directory(dossierSons)) {
            for (const auto& entree : fs::directory_iterator(dossierSons)) {
                if (!entree.is_regular_file()) continue;

                std::string ext = entree.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                if (ext != ".wav") continue;

                SDL_AudioSpec spec;
                Uint8* buf = nullptr;
                Uint32 len = 0;
                if (!SDL_LoadWAV(entree.path().string().c_str(), &spec, &buf, &len)) {
                    std::cout << "Erreur chargement " << entree.path().string() << " : " << SDL_GetError() << std::endl;
                    continue;
                }

                // Convertit le son vers le format du périphérique (fréquence, canaux, format)
                SDL_AudioStream* flux = SDL_NewAudioStream(spec.format, spec.channels, spec.freq,
                                                           obtenu.format, obtenu.channels, obtenu.freq);
                if (flux) {
                    SDL_AudioStreamPut(flux, buf, len);
                    SDL_AudioStreamFlush(flux);
                    int dispo = SDL_AudioStreamAvailable(flux);
                    if (dispo > 0) {
                        std::vector<Uint8> converti(dispo);
                        SDL_AudioStreamGet(flux, converti.data(), dispo);
                        sons.push_back(std::move(converti));
                    }
                    SDL_FreeAudioStream(flux);
                }
                SDL_FreeWAV(buf);
            }
        }
        std::cout << sons.size() << " son(s) charge(s) depuis le dossier \"" << dossierSons << "\"" << std::endl;
    }

    // Joue un son choisi au hasard parmi ceux du dossier
    auto jouerSon = [&]() {
        if (audio == 0 || sons.empty()) return;
        const std::vector<Uint8>& son = sons[rand() % sons.size()];
        SDL_ClearQueuedAudio(audio);          // repart du début si on clique vite
        SDL_QueueAudio(audio, son.data(), static_cast<Uint32>(son.size()));
    };

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
    const double rayonPeur = 150;       // distance à partir de laquelle les poissons paniquent
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
    int nombrePoissons = 400;
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
            // Clic gauche : obstacle temporaire qui fait fuir les poissons
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                clics.emplace_back(render, nullptr, event.button.x, event.button.y, rayonClic);
                finClics.push_back(SDL_GetTicks() + dureeClic);
                boutonEnfonce = true;
            }
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                boutonEnfonce = false;
            }

            // Clic droit : on ajoute un rocher permanent de taille aléatoire
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
                double rayon = rayonMin + rand() % (rayonMax - rayonMin + 1);
                rochers.emplace_back(render, imageRocher, event.button.x, event.button.y, rayon);
                jouerSon();
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

    if (audio != 0) SDL_CloseAudioDevice(audio);
    SDL_DestroyTexture(imageRocher);
    SDL_DestroyTexture(fond);
    IMG_Quit();
    return 0;
}