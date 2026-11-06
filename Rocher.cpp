#include "Rocher.hpp"
#include <cmath>

Rocher::Rocher(SDL_Renderer* renderer, SDL_Texture* texture,
               double positionX, double positionY, double rayon)
    : positionX(positionX), positionY(positionY), rayon(rayon),
      renderer(renderer), texture(texture) {
}

void Rocher::dessiner() {
    int cx = static_cast<int>(positionX);
    int cy = static_cast<int>(positionY);
    int r = static_cast<int>(rayon);

    if (texture) {
        SDL_Rect dest = { cx - r, cy - r, 2 * r, 2 * r };
        SDL_RenderCopy(renderer, texture, nullptr, &dest);
    } else {
        // Image absente : disque gris pour que les rochers restent visibles
        SDL_SetRenderDrawColor(renderer, 110, 110, 110, 255);
        for (int dy = -r; dy <= r; dy++) {
            int half = static_cast<int>(std::sqrt(static_cast<double>(r * r - dy * dy)));
            SDL_RenderDrawLine(renderer, cx - half, cy + dy, cx + half, cy + dy);
        }
    }
}