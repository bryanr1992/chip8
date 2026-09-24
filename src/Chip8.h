#include <cstdint>

//Not sure if this types need to be 4 bytes in size yet
const unsigned int MEMORY_SIZE = 4096;
const unsigned int KEYPAD_COUNT = 16;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVEL = 16;
const unsigned int SCREEN_WIDTH = 64;
const unsigned int SCREEN_HEIGHT = 32;

class Chip8 {
public:
    Chip8();
private:
    uint8_t m_Memory[MEMORY_SIZE]; //4KB memory 1 byte each address 0-4095
    uint8_t mRegisters[REGISTER_COUNT]; //16 8-bit registers
    uint8_t m_Sp{}; //stack pointer. We have a 16-lvl stack that can be index with 0-15
    uint8_t m_DelayTimer{};
    uint8_t m_AudioTimer{};
    uint16_t m_Stack[STACK_LEVEL]{};
    uint16_t m_Pc{}; //program pointer
    uint16_t m_Index{}; // index register to store mem address. Max address is 0xFFF (12-bits) so we need 16 bits to cover lvl
    uint16_t m_Opcode;
    
public:
    uint8_t m_Keypad[KEYPAD_COUNT]{}; //16 keys keypad
    uint32_t m_screen[SCREEN_WIDTH * SCREEN_HEIGHT]{};

public:
    void LoadRom(const char* filename);
};
