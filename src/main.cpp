#include "Chip8.h"
#include "Client.h"
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
    }
    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
	const char* romFilename = argv[3];

	Client client("CHIP-8 Emulator", SCREEN_WIDTH * videoScale, SCREEN_HEIGHT * videoScale, SCREEN_WIDTH, SCREEN_HEIGHT);

	Chip8 chip8;
	chip8.LoadRom(romFilename);

    int videoPitch = sizeof(chip8.m_Screen[0]) * SCREEN_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit) {
		quit = client.ProcessInput(chip8.m_Keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (delta_time > cycleDelay) {
            lastCycleTime = currentTime;
            chip8.Cycle();
            client.Update(chip8.m_Screen, videoPitch);
        }
	}

    return 0;
}
