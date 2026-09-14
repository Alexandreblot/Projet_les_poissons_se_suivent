class Poisson{
    
}


SDL_Surface* surface = IMG_Load("image.png");
SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
SDL_FreeSurface(surface); // la surface n'est plus utile une fois convertie en texture

// Position et taille de l'image à l'écran
SDL_Rect rect = {100, 100, 64, 64}; // x, y, largeur, hauteur

SDL_RenderClear(renderer);
SDL_RenderCopy(renderer, texture, NULL, &rect); // NULL = toute l'image source
SDL_RenderPresent(renderer);