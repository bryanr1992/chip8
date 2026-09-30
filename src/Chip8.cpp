#include "Chip8.h"
#include <chrono>
#include <cstdint>
#include <cstring>
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
    // We can also choose to not list initialize m_ByteDist and do m_ByteDist = std::uniform_int_distribution<uint8_t>(0, 255U); less efficient though
    m_Pc = START_ADDRESS;

    //load character set into memory one byte at time

    for(int i = 0; i < CHARACTER_COUNT; i++){
        m_Memory[CHARACTER_SET_START_ADDRESS + i] = character_set[i];
    }
    
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8:: OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (int i = 0; i < 0xE; i++ ){
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;



    for (int i = 0; i < 0x65; i++){
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

//CPU CYCLE
void Chip8::Cycle() {
    //Fetch opcode
    m_Opcode = (m_Memory[m_Pc] << 8u) | m_Memory[m_Pc + 1];

    //increment pc
    m_Pc += 2;

    //Decode and Execute
    //Grab upper half of the MSB of the opcode and mask it with 1111 0000 0000
    //Then, we shift it to the right 12 times so that we have 0000 0000 1111
    //What we essenially have now is the first hex digit of our function EG: Fx18 -> F
    //We use this first digit to identify the function we need to cool from the function pointer table
    //Further decoding might happen if we need to go to a different table like Table8 for example
    ((*this).*(table[(m_Opcode & 0xF000u) >> 12u]))();

    //Decrement Delay Timer if it's been incremented
    if (m_DelayTimer > 0) {
        --m_DelayTimer;
    }

    //Decrement Sound Timer if it's been incremented
    if (m_AudioTimer > 0) {
        --m_AudioTimer;
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
//clears the screen
void Chip8::OP_00E0(){
    memset(m_Screen, 0, sizeof(m_Screen));
}
// RET instruction: returns from sburoutine
void Chip8::OP_00EE() {
    //When inside a subRoutine the top of the stack has the address
    //of one instruction past the one that called the subRoutine.
    //So we decrease SP first
    --m_Sp;
    m_Pc = m_Stack[m_Sp];
}
// Jump to instruction address
void Chip8::OP_1nnn(){
    // The OP codes on the CHIP8 are 16 bit long. The first 4 bits tell us the instruction we are meant
    // to execute. Thus, the first thing we do after entering the routine/instruction/method is use a
    // mask to get the remaining 12 bits ONLY which are for the address. (We throw away the top 4 bits) 0xFFFu = 0000 1111 1111 1111
    uint16_t addr = m_Opcode & 0x0FFFu;
    m_Pc = addr;
}
//Call instruction at address
void Chip8::OP_2nnn(){
    uint16_t addr = m_Opcode & 0x0FFFu; // Grab 12 bits for addr
    
    m_Stack[m_Sp] = m_Pc; // store addr of current PC on stack
    ++m_Sp; // increase stack pointer

    m_Pc = addr; // Update PC with address of the subroutine
}
// Skip address of the next instruction if register Vx == byte  kk
void Chip8::OP_3xkk(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u; // Get register number
    uint8_t kk = m_Opcode & 0x00FF;// Get byte of data

    //cmp bytes in register with byte of data
    if (m_Registers[Vx] == kk) {
        m_Pc += 2;
    }
}
// Skip address of the next instruction register vx != byte kk
void Chip8::OP_4xkk(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t xx = m_Opcode & 0x00FFu;

    if (m_Registers[Vx] != xx){
        m_Pc += 2;
    }
}
// Skip address of the next instruction if register vx == register vy
void Chip8::OP_5xy0(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;

    if (m_Registers[Vx] == m_Registers[Vy]){
        m_Pc += 2;
    }
}
// put the value of byte kk into register Vx
void Chip8::OP_6xkk(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t kk = m_Opcode & 0x00FFu;

    m_Registers[Vx] = Vx;
}
// adds the value of kk and Vx and stores in Vx
void Chip8::OP_7xkk(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t kk = m_Opcode & 0x00FFu;

    m_Registers[Vx] = m_Registers[Vx] + kk;
}
// stores the value of Vy in Vx
void Chip8::OP_8xy0(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;

    m_Registers[Vx] = m_Registers[Vy];
}
// Set Vx to Vx OR Vy
void Chip8::OP_8xy1(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;

    m_Registers[Vx] = m_Registers[Vx] | m_Registers[Vy];
}
// Set Vx to Vx AND Vy
void Chip8::OP_8xy2(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;

    m_Registers[Vx] = m_Registers[Vx] & m_Registers[Vy];
}
// Set Vx to Vx XOR Vy
void Chip8::OP_8xy3(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;

    m_Registers[Vx] = m_Registers[Vx] ^ m_Registers[Vy];
}

// Add vx,vy set carry
void Chip8::OP_8xy4(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;

    uint16_t sum = m_Registers[Vx] + m_Registers[Vy];
    // bigger than the max val that can be represented in a byte
    // EG: 0xFFu
    if (sum > 255u){
        m_Registers[0x0F] = 1;
    } else {
        m_Registers[0x0F] = 0;
    }

    m_Registers[Vx] = sum & 0xFFu;
}
// subtract vy from vx set VF to 1 if vx > vy otherwise 0
void Chip8::OP_8xy5(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;
    
    if (m_Registers[Vx] > m_Registers[Vy]) {
        m_Registers[0x0F] = 1; 
    } else {
        m_Registers[0x0F] = 0;
    }

    m_Registers[Vx] = m_Registers[Vx] - m_Registers[Vy];
}
// set VF to 1 if the least significant bit is 1 then divide by 2 (SHIFT RIGHT 1)
void Chip8::OP_8xy6(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    m_Registers[0x0F] = m_Opcode & 0x01u;

    m_Registers[Vx] = m_Registers[Vx] >> 1;
}
// SUB Vx from Vy, store results on Vx
void Chip8::OP_8xy7(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;
    
    if (m_Registers[Vy] > m_Registers[Vx]) {
        m_Registers[0x0F] = 1; 
    } else {
        m_Registers[0x0F] = 0;
    }

    m_Registers[Vx] = m_Registers[Vy] - m_Registers[Vx];
}
// Check if MSB is 1 and store in VF. Then multiply by 2 (SHIFT L)
void Chip8::OP_8xyE(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;

    m_Registers[0x0F] = m_Registers[Vx] & 0x80; // 0x80 = 1000 0000

    m_Registers[Vx] = m_Registers[Vx] << 1;
}
// Skip the next instruction if Vx != Vy
void Chip8::OP_9xy0(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0u) >> 4u;

    if (m_Registers[Vx] != m_Registers[Vy]){
        m_Pc += 2;
    }
}
// Set the value nnn to the index register
void Chip8::OP_Annn(){
    uint16_t nnn = m_Opcode & 0x0FFFu;

    m_Index = nnn;
}
// Jump to address nnn + register V0
void Chip8::OP_Bnnn(){
    uint16_t nnn = m_Opcode & 0x0FFFu;

    m_Pc = nnn + m_Registers[0x00];
}
// Set Vx = random byte AND kk
void Chip8::OP_Cxkk(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t kk = m_Opcode & 0x00FFu;

    m_Registers[Vx] = m_ByteDist(m_RandGen) & kk;
}
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = Collision
void Chip8:: OP_Dxyn(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (m_Opcode & 0x00F0) >> 4u;
    uint8_t height = m_Opcode & 0x000Fu; // Height is stored in the last 4 bits

    //Clear Vf
    m_Registers[0x0F] = 0;

    //If over boundary we need to wrap around
    uint8_t xPos = m_Registers[Vx] % SCREEN_WIDTH;
    uint8_t yPos = m_Registers[Vy] % SCREEN_HEIGHT;
    
    for (unsigned int row = 0; row < height; row++){
        uint8_t spriteData = m_Memory[m_Index + row];
        for(unsigned int col = 0; col < 8; col++){
            uint8_t spritePixel = spriteData & (0x80u >> col);

            uint32_t* screenPixel = &m_Screen[(xPos + col) + (yPos + row) * SCREEN_WIDTH]; //Essencially mapping 2D array coord into 1D (row * COL_NUMBER + col)

            //pixel is on?
            if (spritePixel){
                //is the pixel on the screen on? meaning will it collide with the pixel being drawn
                if (*screenPixel == 0xFFFFFFFF){
                    m_Registers[0x0F] = 1;
                }

                *screenPixel = *screenPixel ^ 0xFFFFFFFF;
            }
        }
    }
}
// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
void Chip8::OP_Ex9E(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t key = m_Registers[Vx];

    if (m_Keypad[key]){
        m_Pc += 2;
    }
}
// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
void Chip8::OP_ExA1(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t key = m_Registers[Vx];

    if (!m_Keypad[key]){
        m_Pc += 2;
    }
}
//load the value of the delay timer into Vx
void Chip8::OP_Fx07(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;

    m_Registers[Vx] = m_DelayTimer;
}
//Wait for a key press, store the value of the key in Vx.
//All execution stops until a key is pressed, then the value of that key is stored in Vx.
void Chip8::OP_Fx0A(){
    uint8_t Vx = (m_Opcode & 0x0F00) >> 8u;

    if (m_Keypad[0]) {

        m_Registers[Vx] = 0;

    } else if (m_Keypad[1]) {

        m_Registers[Vx] = 1;

    } else if (m_Keypad[2]) {

        m_Registers[Vx] = 2;

    } else if (m_Keypad[3]) {

        m_Registers[Vx] = 3;

    } else if (m_Keypad[4]) {

        m_Registers[Vx] = 4;

    } else if (m_Keypad[5]) {

        m_Registers[Vx] = 5;

    } else if (m_Keypad[6]) {

        m_Registers[Vx] = 6;

    } else if (m_Keypad[7]) {

        m_Registers[Vx] = 7;

    } else if (m_Keypad[8]) {

        m_Registers[Vx] = 8;

    } else if (m_Keypad[9]) {

        m_Registers[Vx] = 9;

    } else if (m_Keypad[10]) {

        m_Registers[Vx] = 10;

    } else if (m_Keypad[11]) {

        m_Registers[Vx] = 11;

    } else if (m_Keypad[12]) {

        m_Registers[Vx] = 12;

    } else if (m_Keypad[13]) {

        m_Registers[Vx] = 13;

    } else if (m_Keypad[14]) {

        m_Registers[Vx] = 14;

    } else if (m_Keypad[15]) {

        m_Registers[Vx] = 15;

    } else {

        m_Pc -= 2;
    }
}
//Set delay timer = to Vx
void Chip8::OP_Fx15(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;

    m_DelayTimer = m_Registers[Vx];
}
//Set Sount Timer = to Vx
void Chip8::OP_Fx18(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;

    m_AudioTimer = m_Registers[Vx];
}
//Set I = I + Vx
void Chip8::OP_Fx1E(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;

    m_Index = m_Index + m_Registers[Vx];
}
//Set I = location of sprite for digit Vx
//The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
void Chip8::OP_Fx29(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;

    uint8_t digit = m_Registers[Vx];

    m_Index = CHARACTER_SET_START_ADDRESS + (digit * 5);
}
//Store BCD representation of Vx in memory locations I, I+1, and I+2.
//The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, 
//the tens digit at location I+1, and the ones digit at location I+2.
void Chip8:: OP_Fx33(){
    uint8_t Vx = (m_Opcode & 0x0F00u) >> 8u;
    uint8_t val = m_Registers[Vx];

    //Ones place
    m_Memory[m_Index + 2] = val % 10;
    val = val/10; // int division

    //Tens place
    m_Memory[m_Index + 1] = val % 10;
    val = val/10;

    //Hundreds place
    m_Memory[m_Index] = val % 10;

}
//Store registers V0 through Vx in memory starting at location I
void Chip8:: OP_Fx55(){
    uint8_t Vx = (m_Opcode & 0x0F00) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++) {
        m_Memory[m_Index + i] = m_Registers[i];
    }
}
//Read registers V0 through Vx from memory starting at location I.
//The interpreter reads values from memory starting at location I into registers V0 through Vx.
void Chip8::OP_Fx65(){
    uint8_t Vx = (m_Opcode & 0x0F00) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++) {
        m_Registers[i] = m_Memory[m_Index + i];
    }
}

/*
 * FUNCTION POINTERS DEFINITIONS
 * */

void Chip8::Table0(){
    
    ((*this).*table0[m_Opcode & 0x000Fu])();
}

void Chip8::Table8(){
    (*this.*table8[m_Opcode & 0x000Fu])();
}

void Chip8::TableE(){
    ((*this).*tableE[m_Opcode & 0x000Fu])();
}

void Chip8::TableF(){
    ((*this).*tableF[m_Opcode & 0x000Fu])();
}
