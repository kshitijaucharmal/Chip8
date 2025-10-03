#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "fontset.hpp"
struct Chip8 {
    // 4096 bytes memory (RAM)
    std::array<uint8_t, 4096> memory;

    // 16 general-pupose registers
    std::array<uint8_t, 16> registers;

    // Index register (I) (16bit cause Holds memory address)
    uint16_t index_register;

    // Program Counter (Can point anywhere in memory)
    uint16_t program_counter;

    // Stack (for subroutine calls)
    // Using an array as stack does not provide a way to limit to 16
    std::array<uint16_t, 16> stack;

    // Stack Pointer
    uint8_t stack_ptr;

    // Graphics
    std::array<uint8_t, 64 * 32> graphics;

    // Keypad 16 keys (0x0 - 0xF)(0 - not pressed, 1 - pressed)
    std::array<uint8_t, 16> keypad;
    std::array<uint8_t, 16> prev_keypad;

    // Timers 8bit, decrement at 60Hz
    // Delay Timer (DT)
    uint8_t delay_timer;
    // Sound Timer (ST)
    uint8_t sound_timer;

    // Functions
    // Constructor
    explicit Chip8();
    bool loadROM(const std::string &path);

    // Loop Component
    // Fetches instruction from memory at PC
    uint16_t fetchOp();
    // Decode instruction to figure out what to do
    // This will include all opcodes
    // TODO: Decide what this returns (probably an enum)
    void decode(uint16_t instruction);
    // execute the instruction
    // TODO: Take in the decoded value
    void execute();
};
