#include <cstdint>
#include <iostream>
#include <fstream>

class Chip8 {
public:
    uint8_t memory[4096]{}; //4KB memory 1 byte each address 0-4095
    uint8_t registers[16]{}; //16 8-bit registers
    uint8_t sp{}; //stack pointer. We have a 16-lvl stack that can be index with 0-15
    uint8_t delayTimer{};
    uint8_t audioTimer{};
    uint8_t keypad[16]{}; //16 keys keypad
    uint16_t stack[16]{};
    uint16_t pc{}; //program pointer
    uint16_t index{}; // index register to store mem address. Max address is 0xFFF (12-bits) so we need 16 bits to cover lvl
    uint16_t opcode;
    uint32_t screen[64 * 32]{};
};

int main() {
    std::cout << "Hello World of Emulators" << std::endl;
    return 0;
}
