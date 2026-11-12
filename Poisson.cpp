#include "Poisson.hpp"
#include "Rocher.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

Poisson::Poisson(SDL_Renderer* renderer, double vitesse, double direction, double positionX, double positionY, double taille)
    : renderer(renderer), vitesse(vitesse), direction(direction), positionX(positionX), positionY(positionY), taille(taille) {
}

void Poisson::updatePosition() {
    positionX += vitesse * cos(direction);
    positionY += vitesse * sin(direction);

    handleCollisionWithWindowBounds();
}

void Poisson::handleCollisionWithWindowBounds() {
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    const double marge = 20;

    if (positionX < -marge) {
        positionX = windowWidth + marge;
    } else if (positionX > windowWidth + marge) {
        positionX = -marge;
    }

    if (positionY < -marge) {
        positionY = windowHeight + marge;
    } else if (positionY > windowHeight + marge) {
        positionY = -marge;
    }
}

void Poisson::dessiner() {
    int cx = static_cast<int>(positionX);
    int cy = static_cast<int>(positionY);


    
    const int rx = static_cast<int>(20 * taille);        // demi-longueur du corps
    const int ry = static_cast<int>(10 * taille);        // demi-hauteur du corps
    const int tailLen = static_cast<int>(15 * taille);   // longueur de la queue

    // Corps
        // Couleur selon le nombre de voisins : 0 = rouge, voisinsMax ou plus = vert
    const int voisinsMax = 8;
    double t = std::min(nbVoisins, voisinsMax) / static_cast<double>(voisinsMax);   // entre 0 et 1

    Uint8 r = static_cast<Uint8>(255 * std::min(1.0, 2.0 * (1.0 - t)));
    Uint8 g = static_cast<Uint8>(255 * std::min(1.0, 2.0 * t));
    SDL_SetRenderDrawColor(renderer, r, g, 0, 255);
    for (int dy = -ry; dy <= ry; dy++) {
        int half = static_cast<int>(rx * std::sqrt(1.0 - static_cast<double>(dy * dy) / (ry * ry)));
        SDL_RenderDrawLine(renderer, cx - half, cy + dy, cx + half, cy + dy);
    }

    // Queue
    for (int dx = 0; dx <= tailLen; dx++) {
        int h = dx * ry / tailLen;
        SDL_RenderDrawLine(renderer, cx - rx - dx, cy - h, cx - rx - dx, cy + h);
    }

    // Œil (un seul pixel blanc à cette taille)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, cx + rx / 2, cy - ry / 3);
}

void Poisson::suivreBanc(const std::vector<Poisson>& banc) {
    const double rayonVision = 40;   // distance à partir de laquelle un voisin compte
    const double distanceMin = 20;    // en dessous, on s'écarte
    const double vitesseVirage = 0.5; // 0 = ne tourne jamais, 1 = tourne d'un coup

    double alignX = 0, alignY = 0;     // somme des directions des voisins
    double centreX = 0, centreY = 0;   // somme des positions des voisins
    double separX = 0, separY = 0;     // poussée pour s'écarter
    int voisins = 0;

    for (const Poisson& autre : banc) {
        if (&autre == this) continue;   // on ne se compare pas à soi-même

        double dx = autre.positionX - positionX;
        double dy = autre.positionY - positionY;
        double dist = std::sqrt(dx * dx + dy * dy);

        if (dist > rayonVision || dist == 0) continue;   // trop loin

        voisins++;
        alignX += std::cos(autre.direction);
        alignY += std::sin(autre.direction);
        centreX += autre.positionX;
        centreY += autre.positionY;

        if (dist < distanceMin) {
            separX -= dx / dist;   // direction opposée au voisin
            separY -= dy / dist;
        }
    }
    nbVoisins = voisins;
    if (voisins == 0) return;   // seul : il continue tout droit

    // Direction vers le centre des voisins (vecteur de longueur 1)
    double versCentreX = centreX / voisins - positionX;
    double versCentreY = centreY / voisins - positionY;
    double norme = std::sqrt(versCentreX * versCentreX + versCentreY * versCentreY);
    if (norme > 0) {
        versCentreX /= norme;
        versCentreY /= norme;
    }

    // Direction souhaitée = direction actuelle + les trois règles
    double voulueX = std::cos(direction)
                   + 1.0 * (alignX / voisins)
                   + 0.5 * versCentreX
                   + 1.5 * separX;
    double voulueY = std::sin(direction)
                   + 1.0 * (alignY / voisins)
                   + 0.5 * versCentreY
                   + 1.5 * separY;

    double angleVoulu = std::atan2(voulueY, voulueX);

    // Différence d'angle ramenée entre -π et +π, pour tourner par le chemin le plus court
    double diff = angleVoulu - direction;
    while (diff > M_PI)  diff -= 2 * M_PI;
    while (diff < -M_PI) diff += 2 * M_PI;

    direction += diff * vitesseVirage;   // on tourne un peu, pas d'un coup
}

void Poisson::eviterRochers(const std::vector<Rocher>& rochers) {
    const double tailleCorps = 8;     // distance entre le centre du poisson et la surface du rocher
    const double margeVision = 35;    // à partir de quelle distance du rocher le poisson commence à tourner
    const double vitesseVirage = 0.35;

    auto ramener = [](double a) {     // ramène un angle entre -π et +π
        while (a > M_PI)  a -= 2 * M_PI;
        while (a < -M_PI) a += 2 * M_PI;
        return a;
    };

    for (const Rocher& r : rochers) {
        double dx = positionX - r.positionX;
        double dy = positionY - r.positionY;
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist == 0) { dx = 1; dist = 1; }

        double limite = r.rayon + tailleCorps;
        if (dist > limite + margeVision) continue;   // rocher trop loin

        // Angle qui pointe directement à l'opposé du rocher
        double angleFuite = std::atan2(dy, dx);

        // Deux directions tangentes au rocher : on prend celle qui demande le moins de virage
        double tangenteA = angleFuite + M_PI / 2;
        double tangenteB = angleFuite - M_PI / 2;
        double tangente = (std::fabs(ramener(tangenteA - direction)) < std::fabs(ramener(tangenteB - direction)))
                              ? tangenteA : tangenteB;

        if (dist < limite) {
            // Mur : on remet le poisson sur la surface et on le fait glisser le long du rocher
            positionX = r.positionX + dx / dist * limite;
            positionY = r.positionY + dy / dist * limite;
            direction = tangente;
        } else {
            // Approche : plus on est près, plus on tourne fort vers la tangente
            double force = 1.0 - (dist - limite) / margeVision;   // 0 (loin) -> 1 (contre le rocher)
            direction += ramener(tangente - direction) * force * vitesseVirage;
        }
    }
}