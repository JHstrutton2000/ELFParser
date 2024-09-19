// PS2ELFParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define ELF_FILE "C:\\Users\\redst\\Documents\\GitHub\\Transformers2004\\Mods\\SLUS_206.68"

struct instruction {
    const char* name;
    uint32_t mask;
};

#define REGIMM 1000000

instruction instructions[] = {
    { "ADD",     0b00000100000000000000000000000000 },
    //ADDI
    { "ADDIU",   0b00000000000000000000000000100100 },
    { "ADDU",    0b10000100000000000000000000000000 },
    { "AND",     0b00100100000000000000000000000000 },
    { "ANDI",    0b00000000000000000000000000001100 },
    { "BEQ",     0b00000000000000000000000000001000 },
    { "BEQL",    0b00000000000000000000000000001010 },
    { "BGEZ",    0b00000000000000001000000000100000 },
    { "BGEZAL",  0b00000000000000001000100000100000 },
    { "BGEZALL", 0b00000000000000001100100000100000 },
    { "BGEZL",   0b00000000000000001100000000100000 },
    { "BGTZ",    0b00000000000000000000000000111000 },
    { "BGTZL",   0b00000000000000000000000000111010 },
    { "BLEZ",    0b00000000000000000000000000011000 },
    { "BLEZL",   0b00000000000000000000000000011010 },

    { "BLTZ",    0b00000000000000000000000000100000 },
    { "BLTZAL",  0b00000000000000000000100000100000 },
    { "BLTZALL", 0b00000000000000000100100000100000 },
    { "BLTZL",   0b00000000000000000100000000100000 },
    { "BNE",     0b00000000000000000000000000101000 },
    { "BNEL",    0b00000000000000000000000000101010 },
    { "BREAK",   0b10110000000000000000000000000000 },
    { "DADD",    0b00110100000000000000000000000000 },
    { "DADDI",   0b00000000000000000000000000011000 },
    { "DADDIU",  0b00000000000000000000000000100110 },
    { "DADDU",   0b10110100000000000000000000000000 },
    { "DIV",     0b01011000000000000000000000000000 },
    { "DIVU",    0b11011000000000000000000000000000 },
    { "DSLL",    0b00111000000000000000000000000000 },
    { "DSLL32",  0b00111100000000000000000000000000 },
    { "DSLLV",   0b00101000000000000000000000000000 },
    { "DSRA",    0b11011100000000000000000000000000 },
    { "DSRA32",  0b11111100000000000000000000000000 },
    { "DSRAV",   0b11101000000000000000000000000000 },
    { "DSRL",    0b01011100000000000000000000000000 },
    { "DSRL32",  0b00111000000000000000000000000000 },
    { "DSLL",    0b01101000000000000000000000000000 },
    { "DSUB",    0b01110100000000000000000000000000 },
    { "DSUBU",   0b11110100000000000000000000000000 },
    { "J",       0b00000000000000000000000000010000 },
    { "JAL",     0b00000000000000000000000000110000 },
    //{ "JALR",    0b00100100000000000000000000000000 }, // Duplicate ?? This format will not work...
    { "JR",      0b00001000000000000000000000000000 },
    { "LB",      0b00000000000000000000000000000001 },
    { "LBU",     0b00000000000000000000000000001001 },
    { "LD",      0b00000000000000000000000000111011 },
    { "LDL",     0b00000000000000000000000000010110 },
    { "DSLL",    0b00100100000000000000000000000000 }

};

instruction NOP{ "NOP", 0 };

int main() {

    std::ifstream file(ELF_FILE, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    // Get the size of the file
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a buffer to hold the data
    std::vector<char> buffer(fileSize);

    // Read the file contents into the buffer
    if (file.read(buffer.data(), fileSize)) {
        std::cout << "File read successfully." << std::endl;

        //parse(buffer);
    }
    else {
        std::cerr << "Failed to read the file." << std::endl;
    }

    std::cin;
}


//void parse(std::vector<char> buffer) {
//    for (char b : buffer) {
//        //if ((b & ADDIU_MASK) == ADDIU) {
//
//        //}
//    }
//}