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
    std::array<uint16_t, 16> stack;

    // Stack Pointer
    uint8_t stack_ptr;

    // Graphics
    std::array<uint8_t, 64 * 32> graphics;

    // Keypad 16 keys (0x0 - 0xF)(0 - not pressed, 1 - pressed)
    std::array<uint8_t, 16> key;

    // Timers 8bit, decrement at 60Hz
    // Delay Timer (DT)
    uint8_t delay_timer;
    // Sound Timer (ST)
    uint8_t sound_timer;

    // Functions
    // Constructor
    explicit Chip8();
    bool loadROM(const std::string &path);
};
