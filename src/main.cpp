#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <strings.h>
#include <unordered_map>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdio>

#include "Chip8.hpp"

const int screenwidth = 64;
const int screenheight = 32;
const int tilesize = 20;

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
const SDL_FRect button = {10, 10, 200, 80};
SDL_Color normalColor = {70, 130, 180, 255}; // steel blue
SDL_Color hoverColor = {100, 149, 237, 255}; // lighter blue
SDL_Color activeColor = {30, 144, 255, 255}; // darker blue
SDL_Texture *texture;

Chip8 chip;

const double fps = 700.0;
const double frame_time = 1000.0 / fps;

std::unordered_map<SDL_Keycode, uint8_t> mp;

// Layout:
// 1	2	3	C
// 4	5	6	D
// 7	8	9	E
// A	0	B	F
void initKeyMap() {
    mp[SDLK_1] = 0x1;
    mp[SDLK_2] = 0x2;
    mp[SDLK_3] = 0x3;
    mp[SDLK_4] = 0xC;
    mp[SDLK_Q] = 0x4;
    mp[SDLK_W] = 0x5;
    mp[SDLK_E] = 0x6;
    mp[SDLK_R] = 0xD;
    mp[SDLK_A] = 0x7;
    mp[SDLK_S] = 0x8;
    mp[SDLK_D] = 0x9;
    mp[SDLK_F] = 0xE;
    mp[SDLK_Z] = 0xA;
    mp[SDLK_X] = 0x0;
    mp[SDLK_C] = 0xB;
    mp[SDLK_V] = 0xF;
}

int mapKey(SDL_Keycode key) {
    if (mp.count(key)) {
        return mp[key];
    }
    return -1;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    // Initialize map
    initKeyMap();

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_STREAMING, 64 * tilesize,
                                32 * tilesize);

    // Load rom
    if (argc == 2) {
        // Load ROM
        if (!chip.loadROM(argv[1])) {
            return SDL_APP_FAILURE;
        }

    } else {
        printf("Invalid arguments passed, check file path\n");
        return SDL_APP_FAILURE;
    }

    SDL_SetAppMetadata("Example Renderer Clear", "1.0",
                       "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer(
            "examples/renderer/clear", screenwidth * tilesize,
            screenheight * tilesize, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; // end the program, reporting success to the OS.
    }

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

#include <SDL3/SDL.h>
#include <array>

void drawScreen(SDL_Renderer *renderer,
                const std::array<uint8_t, 64 * 32> &screen, int scaleFactor) {
    // Clear screen (black background)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Set draw color for pixels (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Draw scaled pixels
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            int index = y * 64 + x;
            if (screen[index] == 1) {
                SDL_FRect pixelRect = {static_cast<float>(x * scaleFactor),
                                       static_cast<float>(y * scaleFactor),
                                       static_cast<float>(scaleFactor),
                                       static_cast<float>(scaleFactor)};
                SDL_RenderFillRect(renderer, &pixelRect);
            }
        }
    }

    // Present the rendered frame
    SDL_RenderPresent(renderer);
}

void beep_callback(void *userdata, Uint8 *stream, int len) {
    static double phase = 0.0;
    double frequency = 440.0; // A4
    int sample_rate = 44100;

    for (int i = 0; i < len; i++) {
        stream[i] = (Sint8)(std::sin(phase) * 127); // 8-bit signed wave
        phase += 2.0 * 3.14 * frequency / sample_rate;
        if (phase > 2.0 * 3.14)
            phase -= 2.0 * 3.14;
    }
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    uint64_t start = SDL_GetTicks();

    // Update
    // Chip 8 keypad -------------
    const bool *keyStates = SDL_GetKeyboardState(NULL);
    for (auto &[keycode, chipKey] : mp) {
        SDL_Scancode sc = SDL_GetScancodeFromKey(keycode, NULL);
        chip.prev_keypad[chipKey] = chip.keypad[chipKey]; // remember old
        chip.keypad[chipKey] = keyStates[sc] ? 1 : 0;
    }

    // Chip 8 Update loop -------------
    auto instruction = chip.fetchOp();
    chip.decode(instruction);
    // --------------------------------

    drawScreen(renderer, chip.graphics, tilesize);

    // Frame Linting
    uint64_t elapsed = SDL_GetTicks() - start;
    if (elapsed < frame_time) {
        SDL_Delay((uint32_t)(frame_time - elapsed));
    }

    // Delay timer and Sound timer dec
    if (chip.delay_timer > 0)
        chip.delay_timer--;
    if (chip.sound_timer > 0) {
        chip.sound_timer--;
        if (chip.sound_timer == 0) {
            // stop buzzer here
        }
    }

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
}
