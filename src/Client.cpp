#include "Client.h"
#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>

Client::Client(const char* title, int width, int height, int textureWidth, int textureHeight){
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        std::exit(1);
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        std::exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        std::exit(1);
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        textureWidth, textureHeight);
    if (!texture) {
        std::fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        std::exit(1);
    }
}

Client::~Client(){
    SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Client::Update(const void *buffer, int pitch){
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool Client::ProcessInput(uint8_t* keys){

	bool quit = false;

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {

			case SDL_QUIT: {

				quit = true;
			} break;

			case SDL_KEYDOWN: {

				switch (event.key.keysym.sym) {

					case SDLK_ESCAPE: {

						quit = true;
					} break;

					case SDLK_x: {

						keys[0] = 1;
					} break;

					case SDLK_1: {

						keys[1] = 1;
					} break;

					case SDLK_2: {

						keys[2] = 1;
					} break;

					case SDLK_3: {

						keys[3] = 1;
					} break;

					case SDLK_q: {

						keys[4] = 1;
					} break;

					case SDLK_w: {

						keys[5] = 1;
					} break;

					case SDLK_e: {

						keys[6] = 1;
					} break;

					case SDLK_a: {

						keys[7] = 1;
					} break;

					case SDLK_s: {

						keys[8] = 1;
					} break;

					case SDLK_d: {

						keys[9] = 1;
					} break;

					case SDLK_z: {

						keys[0xA] = 1;
					} break;

					case SDLK_c: {

						keys[0xB] = 1;
					} break;

					case SDLK_4: {

						keys[0xC] = 1;
					} break;

					case SDLK_r: {

						keys[0xD] = 1;
					} break;

					case SDLK_f: {

						keys[0xE] = 1;
					} break;

					case SDLK_v: {

						keys[0xF] = 1;
					} break;
				}
			} break;

			case SDL_KEYUP: {

				switch (event.key.keysym.sym) {
					case SDLK_x: {

						keys[0] = 0;
					} break;

					case SDLK_1: {

						keys[1] = 0;
					} break;

					case SDLK_2: {

						keys[2] = 0;
					} break;

					case SDLK_3: {

						keys[3] = 0;
					} break;

					case SDLK_q: {

						keys[4] = 0;
					} break;

					case SDLK_w: {

						keys[5] = 0;
					} break;

					case SDLK_e: {

						keys[6] = 0;
					} break;

					case SDLK_a: {

						keys[7] = 0;
					} break;

					case SDLK_s: {

						keys[8] = 0;
					} break;

					case SDLK_d: {

						keys[9] = 0;
					} break;

					case SDLK_z: {

						keys[0xA] = 0;
					} break;

					case SDLK_c: {

						keys[0xB] = 0;
					} break;

					case SDLK_4: {

						keys[0xC] = 0;
					} break;

					case SDLK_r: {

						keys[0xD] = 0;
					} break;

					case SDLK_f: {

						keys[0xE] = 0;
					} break;

					case SDLK_v: {

						keys[0xF] = 0;
					} break;
				}
			} break;
		}
	}

	return quit;
}
