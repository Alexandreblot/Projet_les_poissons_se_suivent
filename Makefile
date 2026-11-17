poisson: main.cpp Poisson.cpp SDL.cpp Rocher.cpp
	g++ -std=c++17 main.cpp Poisson.cpp SDL.cpp Rocher.cpp -o poisson `pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf`

clean:
	rm -f poisson