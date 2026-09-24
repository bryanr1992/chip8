#include "Chip8.h"
#include <iostream>
#include <fstream>

// starting address of ROM instructions
const unsigned int START_ADDRESS = 0x200;
 
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
