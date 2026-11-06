poisson: main.cpp Poisson.cpp SDL.cpp
	g++ -std=c++17 main.cpp Poisson.cpp SDL.cpp -o poisson `pkg-config --cflags --libs sdl2 SDL2_image`

clean:
	rm -f poisson