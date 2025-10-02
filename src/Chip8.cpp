#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <vector>

#include "Chip8.hpp"
Chip8::Chip8() {
    // Program loads at 0x200 = 512 (Dec)
    program_counter = 0x200;

    // Clear all
    memory.fill(0);
    registers.fill(0);
    graphics.fill(0);
    key.fill(0);
    stack.fill(0);

    // Reset Special Registers
    index_register = 0;
    stack_ptr = 0;

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    // Load fontset at 0x50 (80)
    for (std::size_t i = 0; i < chip8_fontset.size(); ++i) {
        memory[0x50 + i] = chip8_fontset[i];
    }
}

bool Chip8::loadROM(const std::string &path) {
    std::ifstream rom(path.c_str(), std::ios::in | std::ios::binary);

    if (!rom.is_open()) {
        printf("File not found: %s\n", path.c_str());
        return false;
    }

    // Go to end
    rom.seekg(0, std::ios::end);
    // Get size
    std::streamsize size = rom.tellg();
    // Go back
    rom.seekg(0, std::ios::beg);

    printf("Size: %td\n", size);

    if (size <= 0 || size > (4096 - 0x200)) {
        printf("ROM too big, should be 0x%04X max\n", (4096 - 0x200));
        return false;
    }

    // Buffer of filesize
    std::vector<char> buffer(static_cast<size_t>(size));

    // If read fails
    if (!rom.read(buffer.data(), size))
        return false;

    // Load at program counter loc (0x200)
    for (size_t i = 0; i < buffer.size(); i++) {
        memory[program_counter + i] = buffer[i];
    }
    return true;
}

uint16_t Chip8::fetchOp() {
    uint8_t i1 = memory[program_counter];
    uint8_t i2 = memory[program_counter + 1];
    uint16_t current_op = (static_cast<uint16_t>(i1) << 8) | i2;

    // increment pc
    program_counter += 2;

    return current_op;
}

void Chip8::decode(uint16_t instruction) {
    uint8_t D = (instruction & 0xF000) >> 12;
    uint8_t X = (instruction & 0x0F00) >> 8;
    uint8_t Y = (instruction & 0x00F0) >> 4;
    uint8_t N = (instruction & 0x000F);

    // Get the last three nibbles in one, useful for many instructions
    uint16_t tribble = (X << 8) | (Y << 4) | N;

    // Print the instruction
    printf("%X%X%X%X: ", D, X, Y, N);

    // TODO: Implement these in execute, instruction type -> enum
    //
    // Clear Screen 0x00E0
    if (instruction == 0x00E0) {
        printf("Clear Screen\n");
        // Clear Screen
        graphics.fill(0);
    }
    // Jump to location, 1NNN
    else if (D == 0x1) {
        printf("Set PC to %04X\n", tribble);
        program_counter = tribble;
    }
    // Call a subroutine 2NNN
    else if (D == 0x2) {
        // TODO: Check if stack filled
        // Pushing to stack
        stack[stack_ptr++] = program_counter;

        program_counter = tribble;
    }
    // Return from a subroutine
    else if (D == 0x00EE) {
        // Read the top
        program_counter = stack[stack_ptr - 1];
        // Set it back to zero
        stack[stack_ptr - 1] = 0;
        // get the ptr down
        stack_ptr--;
    } else {
        printf("\n");
    }
}
