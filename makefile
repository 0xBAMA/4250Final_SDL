CC = g++

MAKE_EXE = -o exe -time

SDL_FLAGS = $(shell pkg-config sdl2 --cflags --libs)

GL_FLAGS = -lglut -lGLEW -lGL -lGLU

LODEPNG_FLAGS = resources/LodePNG/lodepng.cpp -ansi -O3 -std=c++11

#UNNECCESARY_DEBUG = -Wall -Wextra -pedantic

all: build

build: main.cc
	$(CC) main.cc $(MAKE_EXE) $(GL_FLAGS) $(LODEPNG_FLAGS) $(SDL_FLAGS)
