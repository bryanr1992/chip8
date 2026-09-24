#include "Chip8.h"
#include <chrono>
#include <iostream>
#include <fstream>

// starting address of ROM instructions
constexpr unsigned int START_ADDRESS = 0x200;

//count in bytes for the character set of CHIP8
constexpr unsigned int CHARACTER_COUNT = 80;

//character_set start address
constexpr unsigned int CHARACTER_SET_START_ADDRESS = 0x50;

uint8_t character_set[CHARACTER_COUNT] = {
     0xF0,  0x90,  0x90,  0x90,  0xF0, //0
     0x20,  0x60,  0x20,  0x20,  0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//number 1 example
/* 00100000 //0x20
 * 01100000 //0x60
 * 00100000 //0x20
 * 00100000 //0x20
 * 01110000 //0x70
 * */

//First we do member initialization list for RNG 
Chip8::Chip8()
    : m_RandGen(std::chrono::system_clock::now().time_since_epoch().count()),
      m_ByteDist()//range(0,255u). Equivalent to m_ByteDist(0, 255u). See HEADER for clarity
{
    m_Pc = START_ADDRESS;

    //load character set into memory one byte at time

    for(int i = 0; i < CHARACTER_COUNT; i++){
        m_Memory[CHARACTER_SET_START_ADDRESS + i] = character_set[i];
    }

}
 
//load rom
void Chip8::LoadRom(const char* filename) {
        
    //open rom file as a stream of binary data and move the file pointer to EOF
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open()) {
        std::streampos size = file.tellg(); //get size of file
        char* buffer = new char[size]; //allocate buffer based on size

        //go back to file beginning and load the buffer with data
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        //load the contents of the ROM into the CHIP8 memory

        for(long i = 0; i < size; i++) {
            m_Memory[START_ADDRESS + i] = buffer[i];
        }
        //free the buffer
        delete[] buffer;
    }
}

int main() {
    std::cout << "Hello World of Emulators" << std::endl;
    return 0;
}
