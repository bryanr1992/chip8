#include "Chip8.h"
#include "Client.h"
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    //int videoScale = std::stoi("400");
    int videoScale = 20;
    int cycleDelay = std::stoi("1");
	//char const* romFilename = argv[3];

	Client client("CHIP-8 Emulator", SCREEN_WIDTH * videoScale, SCREEN_HEIGHT * videoScale, SCREEN_WIDTH, SCREEN_HEIGHT);

	Chip8 chip8;
	//chip8.LoadROM(romFilename);
    int videoPitch = sizeof(chip8.m_Screen[0]) * SCREEN_WIDTH;

	//auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit) {
		quit = client.ProcessInput(chip8.m_Keypad);
		client.Update(chip8.m_Screen, videoPitch);
	}

    return 0;
}
