#include <cstdio>

#include "Chip8.hpp"

const int screenwidth = 64;
const int screenheight = 32;
const int tilesize = 8;

int main(int argc, char *argv[]) {

    if (argc == 2) {
        Chip8 chip;
        // Load ROM
        if (!chip.loadROM(argv[1])) {
            return 1;
        }

        // Ahead

    } else {
        printf("Invalid arguments passed, check file path\n");
    }

    return 0;
}
