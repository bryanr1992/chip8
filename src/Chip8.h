#pragma once

#include <cstdint>
#include <random>

//Not sure if this types need to be 4 bytes in size yet
constexpr unsigned int MEMORY_SIZE = 4096;
constexpr unsigned int KEYPAD_COUNT = 16;
constexpr unsigned int REGISTER_COUNT = 16;
constexpr unsigned int STACK_LEVEL = 16;
constexpr unsigned int SCREEN_WIDTH = 64;
constexpr unsigned int SCREEN_HEIGHT = 32;

class Chip8 {
public:
    Chip8();
private:
    uint8_t m_Memory[MEMORY_SIZE]; //4KB memory 1 byte each address 0-4095
    uint8_t m_Registers[REGISTER_COUNT]; //16 8-bit registers
    uint8_t m_Sp{}; //stack pointer. We have a 16-lvl stack that can be index with 0-15
    uint8_t m_DelayTimer{};
    uint8_t m_AudioTimer{};
    uint16_t m_Stack[STACK_LEVEL]{};
    uint16_t m_Pc{}; //program pointer
    uint16_t m_Index{}; // index register to store mem address. Max address is 0xFFF (12-bits) so we need 16 bits to cover lvl
    uint16_t m_Opcode;// Stores CHIP8 CPU current instruction
private: 
    void OP_00E0(); //cls
    void OP_00EE(); //RET
    void OP_1nnn(); //JP Addr
    void OP_2nnn(); //CALL Addr
    void OP_3xkk(); // SE Vx, byte
    void OP_4xkk(); // SNE Vx, byte
    void OP_5xy0(); // SE Vx, Vy
    void OP_6xkk(); // LD, Vc, byte
    void OP_7xkk(); // Add Vx, byte
    void OP_8xy0(); //LD vx, vy
    void OP_8xy1(); // OR vx, vy
    void OP_8xy2(); // AND vx, vy
    void OP_8xy3(); // XOR vx, vy
    void OP_8xy4(); // ADD vx,vy set carr
    void OP_8xy5(); // SUB vx, vy
    void OP_8xy6(); // SHR Vx, by 1
    void OP_8xy7(); // SUBN vx, vy
    void OP_8xyE(); // SHL, vx, by 1
    void OP_9xy0(); // SNE vx, vy
    void OP_Annn(); // LD I, address
    void OP_Bnnn(); // JP, to nnn + v0
    void OP_Cxkk(); // RND Vx, byte
    void OP_Dxyn(); // DRW Vx, Vy, nibble
    void OP_Ex9E(); // SKP Vx
    void OP_ExA1(); // SKNP Vx
    void OP_Fx07(); // LD Vx, DT
    void OP_Fx0A(); // LD Vx, k
    void OP_Fx15(); // LD DT, Vx
    void OP_Fx18(); // LD ST, Vx
    void OP_Fx1E(); // ADD I, Vx
    void OP_Fx29(); // LD F, Vx
    void OP_Fx33(); // LD B, Vx
    void OP_Fx55(); // LD [I], Vx
    void OP_Fx65(); // LD Vx, [i]

    //used to generate random seed
    std::default_random_engine m_RandGen;
    std::uniform_int_distribution<unsigned int> m_ByteDist;

    //function pointer table
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OP_NULL();

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];


public:
    uint8_t m_Keypad[KEYPAD_COUNT]{}; //16 keys keypad
    uint32_t m_Screen[SCREEN_WIDTH * SCREEN_HEIGHT]{};

public:
    void Cycle();
    void LoadRom(const char* filename);
};
