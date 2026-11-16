poisson: main.cpp Poisson.cpp SDL.cpp Rocher.cpp Poisson.hpp SDL.hpp Rocher.hpp
	g++ -std=c++17 main.cpp Poisson.cpp SDL.cpp Rocher.cpp -o poisson `pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer`

clean:
	rm -f poisson