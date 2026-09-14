poisson: main.cpp Poisson.cpp SDL.cpp
	g++ -std=c++17 main.cpp Poisson.cpp SDL.cpp -o poisson `pkg-config --cflags --libs sdl2`

clean:
	rm -f poisson

