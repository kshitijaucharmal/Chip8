#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <random>
#include <strings.h>
#include <vector>

#include "Chip8.hpp"
Chip8::Chip8() {
    // Program loads at 0x200 = 512 (Dec)
    program_counter = 0x200;

    // Clear all
    memory.fill(0);
    registers.fill(0);
    graphics.fill(0);
    keypad.fill(0);
    prev_keypad.fill(0);
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

bool Chip8::isKeyPressedOnce(uint8_t key) {
    return keypad[key] && !prev_keypad[key];
}

uint16_t Chip8::fetchOp() {
    uint8_t i1 = memory[program_counter];
    uint8_t i2 = memory[program_counter + 1];
    uint16_t current_op = (static_cast<uint16_t>(i1) << 8) | i2;

    // increment pc
    program_counter += 2;

    return current_op;
}

// void drawScreen(const std::array<uint8_t, 64 * 32> screen) {
//     for (int y = 0; y < 32; y++) {
//         for (int x = 0; x < 64; x++) {
//             int index = y * 64 + x;
//             if (screen[index] == 1)
//                 printf("██"); // pixel ON
//             else
//                 printf("  "); // pixel OFF
//         }
//         printf("\n"); // new row
//     }
// }

void Chip8::decode(uint16_t instruction) {
    uint8_t D = (instruction & 0xF000) >> 12;
    uint8_t X = (instruction & 0x0F00) >> 8;
    uint8_t Y = (instruction & 0x00F0) >> 4;
    uint8_t N = (instruction & 0x000F);

    // Get the last three nibbles in one, useful for many instructions
    uint16_t tribble = (X << 8) | (Y << 4) | N;
    // Get the last two nibbles in one
    uint8_t dibble = (Y << 4) | N;

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
        printf("Jump to %04X\n", tribble);
        program_counter = tribble;
    }
    // 2NNN Call a subroutine
    else if (D == 0x2) {
        // TODO: Check if stack filled
        // Pushing to stack
        printf("Push PC (%04X) to stack and Jump to %X\n", program_counter,
               tribble);
        stack[stack_ptr++] = program_counter;

        program_counter = tribble;
    }
    // 00EE Return from a subroutine
    else if (instruction == 0x00EE) {
        // Read the top
        program_counter = stack[stack_ptr - 1];
        // Set it back to zero
        stack[stack_ptr - 1] = 0;
        // get the ptr down
        stack_ptr--;

        printf("Go Back\n");
    }
    // ANNN set index register to NNN
    else if (D == 0xA) {
        printf("Set I to %X\n", tribble);
        index_register = tribble;
    }
    // BNNN Jump to NNN + V0
    else if (D == 0xB) {
        printf("Jump to %04X (tribble + V0)\n", (tribble + registers[0x0]));
        program_counter = tribble + registers[0x0];
    }
    // CxNN Random byte & dibble
    else if (D == 0xC) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> distrib(0, 255);

        uint8_t random_byte = distrib(gen);

        printf("Random Byte: %X\n", random_byte);
        registers[X] = random_byte & dibble;
    }
    // 3xkk - Skip Next (PC += 2) if Vx = kk
    else if (D == 0x3) {
        if (dibble == registers[X]) {
            printf("%02X == %02X, skipping next instruction.\n", dibble,
                   registers[X]);
            program_counter += 2;
        } else
            printf("%02X != %02X, nothing to do.\n", dibble, registers[X]);
    }
    // 4xkk - Skip Next (PC += 2) if Vx != kk
    else if (D == 0x4) {
        if (dibble != registers[X]) {
            printf("%02X != %02X, skipping next instruction.\n", dibble,
                   registers[X]);
            program_counter += 2;
        }
        printf("%02X == %02X, nothing to do.\n", dibble, registers[X]);
    }
    // 5xy0 - Skip Next (PC += 2) if Vx == Vy
    else if (D == 0x5) {
        if (registers[X] == registers[Y]) {
            printf("%02X == %02X, skipping next instruction.\n", registers[X],
                   registers[Y]);
            program_counter += 2;
        }
        printf("%02X != %02X, nothing to do.\n", registers[X], registers[Y]);
    }
    // 6xkk - Set Vx = kk
    else if (D == 0x6) {
        printf("Setting register %X to %X\n", X, dibble);
        registers[X] = dibble;
    }
    // 7xkk - Add kk to Vx
    else if (D == 0x7) {
        printf("Adding %X to register %X = %X\n", dibble, registers[X],
               (registers[X] + dibble) & 0xFF);
        registers[X] += dibble;
    }
    // DXYN - Display
    else if (D == 0xD) {
        printf("Drawing Sprite\n");

        registers[0xF] = 0;

        // For N rows
        for (auto row = 0; row < N; row++) {
            uint8_t yPos = registers[Y] + row;
            // If going outside bottom side of screen
            if (yPos >= 32)
                break;
            uint8_t sprite_data = memory[index_register + row];
            for (int col = 0; col < 8; col++) {
                // If going outside right side of screen
                uint8_t xPos = registers[X] + col;
                if (xPos >= 64)
                    break;
                // Check if sprite data is set (Check from MSB)
                if ((sprite_data & (0x80 >> col)) != 0) {
                    // Index in graphics array
                    size_t index = yPos * 64 + xPos;

                    // Collision (VF = 1)
                    if (graphics[index] == 1) {
                        registers[0xF] = 1;
                    }

                    // Graphics set to XOR of value
                    graphics[index] ^= 1;
                }
            }
        }
    }
    // 8xyn - Arithematic Operations
    else if (D == 0x8) {
        printf("Arithematic Operation\n");
        auto &vx = registers[X];
        auto &vy = registers[Y];

        switch (N) {
        case 0x0:
            vx = vy;
            break;
        case 0x1:
            vx = vx | vy;
            break;
        case 0x2:
            vx = vx & vy;
            break;
        case 0x3:
            vx = vx ^ vy;
            break;
        case 0x4: {
            uint16_t sum = vx + vy;
            registers[0xF] = (sum > 0xFF);
            vx = sum & 0xFF;
            break;
        }
        case 0x5:
            if (vx > vy)
                registers[0xF] = 1;
            else
                registers[0xF] = 0;

            vx = vx - vy;
            break;
        case 0x6:
            registers[0xF] = vx & 0x1;
            vx = vx >> 1;
            break;
        case 0x7:
            if (vy > vx)
                registers[0xF] = 1;
            else
                registers[0xF] = 0;
            vx = vy - vx;
            break;
        case 0xE:
            registers[0xF] = (vx & 0x80) >> 7;
            vx = vx << 1;
            break;
        default:
            break;
        }
    }
    // 9xy0 - Skip if Vx != Vy
    else if (D == 0x9) {
        printf("Skip if Vx != Vy\n");
        if (registers[X] != registers[Y]) {
            program_counter += 2;
        }
    }
    // Ex91 and ExA1 (Keyboard)
    else if (D == 0xE) {
        if (dibble == 0x9E) {
            printf("Dibble 0x9E\n");
            if (keypad[registers[X]])
                program_counter += 2;
        }
        if (dibble == 0xA1) {
            printf("Dibble 0xA1\n");
            if (!keypad[registers[X]])
                program_counter += 2;
        }
    }
    // FxNN
    else if (D == 0xF) {
        // 07 - Vx = delay timer
        if (dibble == 0x07) {
            registers[X] = delay_timer;
        }
        // 0A - wait for key press, store key in Vx
        else if (dibble == 0x0A) {
            bool keyPressed = false; // track if any key was pressed this frame
            for (int k = 0; k < 16; k++) {
                if (isKeyPressedOnce(k)) {
                    registers[X] = k; // store key in register
                    waitForKey = false;
                    keyPressed = true;
                    break;
                }
            }

            if (!keyPressed) {
                // stay on this instruction until a key is pressed
                program_counter -= 2;
                // optional: print debug
                printf("Waiting for key press...\n");
            } else {
                // optional: debug print
                printf("Key %d pressed\n", registers[X]);
            }
        }
        // 15 - Set delay timer = Vx
        else if (dibble == 0x15) {
            delay_timer = registers[X];
        }
        // 18 - Set sound timer = Vx
        else if (dibble == 0x18) {
            sound_timer = registers[X];
        }
        // 1E - I = I + Vx
        else if (dibble == 0x1E) {
            index_register += registers[X];
        }
        // 29 - Set I = location of sprite (font) for digit Vx
        else if (dibble == 0x29) {
            // x5 cause each sprite takes 5 bytes
            index_register = 0x50 + ((registers[X] & 0xF) * 5);
        }
        // 33 - Store BCD in I, I+1, I+2
        else if (dibble == 0x33) {
            uint8_t value = registers[X];
            memory[index_register] = value / 100;           // hundreds digit
            memory[index_register + 1] = (value / 10) % 10; // tens digit
            memory[index_register + 2] = value % 10;
        }
        // 55 - Stores register from V0 to Vx in memory starting from I
        else if (dibble == 0x55) {
            for (int i = 0; i <= X; i++) {
                memory[index_register + i] = registers[i];
            }
        }
        // 65 - Read memory from registers
        else if (dibble == 0x65) {
            for (int i = 0; i <= X; i++) {
                registers[i] = memory[index_register + i];
            }
        }
    }
    // Jump to the next line
    else {
        printf("\n");
    }
}
