# Chip8

A simple Chip 8 emulator written in cpp for educational purposes

Chip8 Specifications ([source](https://www.arjunnair.in/p12/)):
- The Chip 8 has 4k of addressable memory. The first 512 bytes are reserved (originally for the interpreter) and should not be used.
- There are 16 8-bit registers numbered, traditionally, as V0, V1 … to VF (hexadecimal). VF is a special flag register that’s modified by certain instructions.
- Chip 8 also employs two special 8 bit registers that act as a general delay timer (DT) and Sound timer (ST). Both are decremented at rate of 60HZ when they are non-zero. A sound tone is emitted in the case of ST whenever it’s decremented.
- The display is monochrome and of 64x32 pixel resolution. The origin (0,0) is at the top left.
- There are 36 instructions in the Chip 8 language. All of them are 2 bytes long and are of the Big Endian form.
- Interestingly for a valid instruction, it’s first byte should be at an even address.
- The keyboard is very simple and consists of the Hexadecimal numbers from 0 to F.
- Two pseudo-registers PC and SP are not available to programs but control the execution of Chip 8. PC is a 16 bit register that holds the memory address of the current instruction. SP is a 8 bit register that points to an array of 16 16 bit values (memory addresses).
