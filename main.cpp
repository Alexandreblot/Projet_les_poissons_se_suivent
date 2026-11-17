#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "SDL.hpp"
#include "Poisson.hpp"
#include "Rocher.hpp"

// ---------------------------------------------------------------------------
// Une barre de réglage horizontale qui modifie un paramètre (via un pointeur)
// ---------------------------------------------------------------------------
struct Slider {
    std::string nom;
    double* valeur;        // le paramètre que la barre modifie
    double vMin, vMax;
    SDL_Color couleur;
    SDL_Rect rail;

    double ratio() const { return (*valeur - vMin) / (vMax - vMin); }

    bool contient(int mx, int my) const {
        return mx >= rail.x - 8 && mx <= rail.x + rail.w + 8 &&
               my >= rail.y - 10 && my <= rail.y + rail.h + 10;
    }

    void regler(int mx) {
        double t = static_cast<double>(mx - rail.x) / rail.w;
        t = std::max(0.0, std::min(1.0, t));
        *valeur = vMin + t * (vMax - vMin);
    }

    void dessiner(SDL_Renderer* r) const {
        SDL_SetRenderDrawColor(r, 60, 60, 70, 255);
        SDL_RenderFillRect(r, &rail);

        SDL_Rect rempli = { rail.x, rail.y, static_cast<int>(ratio() * rail.w), rail.h };
        SDL_SetRenderDrawColor(r, couleur.r, couleur.g, couleur.b, 255);
        SDL_RenderFillRect(r, &rempli);

        SDL_Rect curseur = { rail.x + static_cast<int>(ratio() * rail.w) - 4, rail.y - 5, 8, rail.h + 10 };
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderFillRect(r, &curseur);
    }
};

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

    // ---- Police pour les textes du panneau ----
    // On essaie d'abord "police.ttf" (à mettre à côté de l'exécutable), puis des polices du système
    TTF_Init();
    TTF_Font* police = nullptr;
    const char* cheminsPolice[] = {
        "police.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",   // Linux (Debian/Ubuntu)
        "/System/Library/Fonts/Supplemental/Arial.ttf",      // macOS
        "C:/Windows/Fonts/arial.ttf"                         // Windows
    };
    for (const char* chemin : cheminsPolice) {
        police = TTF_OpenFont(chemin, 14);
        if (police) break;
    }
    if (!police) {
        std::cout << "Aucune police trouvee : " << TTF_GetError() << std::endl;
    }

    // Écrit un texte à la position (x, y)
    auto dessinerTexte = [&](const std::string& texte, int x, int y, SDL_Color couleur) {
        if (!police) return;
        SDL_Surface* surf = TTF_RenderUTF8_Blended(police, texte.c_str(), couleur);
        if (!surf) return;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(render, surf);
        SDL_Rect dest = { x, y, surf->w, surf->h };
        SDL_FreeSurface(surf);
        if (tex) {
            SDL_RenderCopy(render, tex, NULL, &dest);
            SDL_DestroyTexture(tex);
        }
    };

    // ---- Sons joués quand on pose un rocher (dossier "sons" : mp3, ogg, wav, flac...) ----
    const std::string dossierSons = "sons";
    std::vector<Mix_Chunk*> sons;
    bool audioOuvert = false;

    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);   // active les décodeurs MP3 et OGG
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Erreur ouverture audio : " << Mix_GetError() << std::endl;
    } else {
        audioOuvert = true;
        Mix_AllocateChannels(16);             // jusqu'à 16 sons en même temps

        namespace fs = std::filesystem;
        if (fs::is_directory(dossierSons)) {
            // On essaie tous les fichiers (sous-dossiers compris) : SDL_mixer regarde le contenu
            for (const auto& entree : fs::recursive_directory_iterator(dossierSons)) {
                if (!entree.is_regular_file()) continue;
                if (entree.path().filename().string()[0] == '.') continue;   // fichiers cachés

                Mix_Chunk* son = Mix_LoadWAV(entree.path().string().c_str());
                if (son) {
                    sons.push_back(son);
                    std::cout << "  son charge : " << entree.path().string() << std::endl;
                } else {
                    std::cout << "Erreur chargement " << entree.path().string() << " : " << Mix_GetError() << std::endl;
                }
            }
        } else {
            std::cout << "Dossier introuvable : " << fs::absolute(dossierSons).string() << std::endl;
        }
        std::cout << sons.size() << " son(s) charge(s) depuis " << fs::absolute(dossierSons).string() << std::endl;
    }

    // Joue un son choisi au hasard parmi ceux du dossier
    auto jouerSon = [&]() {
        if (!audioOuvert || sons.empty()) return;
        Mix_PlayChannel(-1, sons[rand() % sons.size()], 0);   // -1 = premier canal libre
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
    const Uint32 dureeClic = 100;       // durée de vie en millisecondes
    const double rayonPeur = 150;       // distance à partir de laquelle les poissons paniquent
    bool boutonEnfonce = false;         // vrai tant que le bouton gauche est maintenu
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

    // ---- Panneau de réglages à droite ----
    ParametresBanc params;              // rayon de vision, poids, etc. (défini dans Poisson.hpp)
    double vitesse = 3;                 // vitesse de tous les poissons
    int sliderActif = -1;               // indice de la barre qu'on tient (-1 = aucune)

    const int panneauL = 220;
    SDL_Rect panneau = { largeur - panneauL - 10, 10, panneauL, hauteur - 20 };

    std::vector<Slider> sliders;
    auto ajouterSlider = [&](const std::string& nom, double* valeur, double vMin, double vMax, SDL_Color couleur) {
        int i = static_cast<int>(sliders.size());
        // 45 pixels sous le haut du panneau (pour le titre), puis 50 pixels entre chaque barre
        // (la place au-dessus de chaque barre sert au texte)
        SDL_Rect rail = { panneau.x + 15, panneau.y + 55 + i * 50, panneau.w - 30, 10 };
        sliders.push_back({ nom, valeur, vMin, vMax, couleur, rail });
    };

    ajouterSlider("Rayon de vision",   &params.rayonVision,      20,   250, {255, 255, 255, 255});  // blanc
    ajouterSlider("Distance de securite", &params.distanceMin,   5,    80,  {255, 100, 100, 255});  // rouge
    ajouterSlider("Vitesse de virage", &params.vitesseVirage,    0.01, 0.5, {100, 200, 255, 255});  // bleu
    ajouterSlider("Alignement",        &params.poidsAlignement,  0,    3,   {100, 255, 100, 255});  // vert
    ajouterSlider("Cohesion",          &params.poidsCohesion,    0,    3,   {255, 220, 80,  255});  // jaune
    ajouterSlider("Separation",        &params.poidsSeparation,  0,    5,   {255, 150, 50,  255});  // orange
    ajouterSlider("Vitesse des poissons", &vitesse,              0.5,  8,   {200, 120, 255, 255});  // violet

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            // Clic gauche : soit on attrape une barre, soit on crée un obstacle
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mx = event.button.x, my = event.button.y;

                int touche = -1;
                for (size_t i = 0; i < sliders.size(); i++) {
                    if (sliders[i].contient(mx, my)) touche = static_cast<int>(i);
                }
                bool dansPanneau = mx >= panneau.x && mx <= panneau.x + panneau.w &&
                                   my >= panneau.y && my <= panneau.y + panneau.h;

                if (touche >= 0) {
                    sliderActif = touche;
                    sliders[touche].regler(mx);
                } else if (!dansPanneau) {   // un clic dans le panneau ne crée pas d'obstacle
                    clics.emplace_back(render, nullptr, event.button.x, event.button.y, rayonClic);
                    finClics.push_back(SDL_GetTicks() + dureeClic);
                    boutonEnfonce = true;
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                boutonEnfonce = false;
                sliderActif = -1;
            }

            // Mouvement de la souris pendant qu'on tient une barre
            if (event.type == SDL_MOUSEMOTION && sliderActif >= 0) {
                sliders[sliderActif].regler(event.motion.x);
            }

            // Clic droit : on ajoute un rocher permanent de taille aléatoire + un son
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
                double rayon = rayonMin + rand() % (rayonMax - rayonMin + 1);
                rochers.emplace_back(render, imageRocher, event.button.x, event.button.y, rayon);
                jouerSon();
            }
        }

        // Nom et valeur de la barre tenue : affichés dans le titre de la fenêtre
        if (sliderActif >= 0) {
            char titre[64];
            std::snprintf(titre, sizeof(titre), "%s : %.2f",
                          sliders[sliderActif].nom.c_str(), *sliders[sliderActif].valeur);
            SDL_SetWindowTitle(SDL_RenderGetWindow(render), titre);
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
            p.suivreBanc(poissons, params);
            p.eviterRochers(rochers);
            p.eviterRochers(clics);
            for (auto& c : clics) {
                p.fuir(c.positionX, c.positionY, rayonPeur);
            }
            p.vitesse = vitesse;
            p.updatePosition();
            p.dessiner();
        }

        // Panneau de réglages, dessiné par-dessus tout le reste
        SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(render, 20, 20, 30, 200);   // fond sombre semi-transparent
        SDL_RenderFillRect(render, &panneau);
        SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
        SDL_RenderDrawRect(render, &panneau);               // contour

        dessinerTexte("Reglages du banc", panneau.x + 15, panneau.y + 12, {255, 255, 255, 255});

        for (const auto& s : sliders) {
            s.dessiner(render);

            // Texte au-dessus de la barre : nom du réglage et valeur actuelle
            char texte[64];
            std::snprintf(texte, sizeof(texte), "%s : %.2f", s.nom.c_str(), *s.valeur);
            dessinerTexte(texte, s.rail.x, s.rail.y - 26, s.couleur);
        }

        SDL_RenderPresent(render);
        SDL_Delay(16);
    }

    for (Mix_Chunk* son : sons) {
        Mix_FreeChunk(son);
    }
    if (audioOuvert) Mix_CloseAudio();
    Mix_Quit();

    if (police) TTF_CloseFont(police);
    TTF_Quit();

    SDL_DestroyTexture(imageRocher);
    SDL_DestroyTexture(fond);
    IMG_Quit();
    return 0;
}