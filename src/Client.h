#pragma once

#include <cstdint>
#include <SDL2/SDL.h>

class Client {
public:
    Client(const char* title, int width, int height, int textureWidth, int textureHeight);
    ~Client();
    void Update(const void* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

};
