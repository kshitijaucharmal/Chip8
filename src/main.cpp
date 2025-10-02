
#include "SDL3/SDL_mouse.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdio>

#include "Chip8.hpp"

const int screenwidth = 64;
const int screenheight = 32;
const int tilesize = 10;

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
const SDL_FRect button = {10, 10, 200, 80};
SDL_Color normalColor = {70, 130, 180, 255}; // steel blue
SDL_Color hoverColor = {100, 149, 237, 255}; // lighter blue
SDL_Color activeColor = {30, 144, 255, 255}; // darker blue

Chip8 chip;

const double fps = 60.0;
const double frame_time = 1000.0 / fps;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (argc == 2) {
        // Load ROM
        if (!chip.loadROM(argv[1])) {
            return SDL_APP_FAILURE;
        }

    } else {
        printf("Invalid arguments passed, check file path\n");
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
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS.
                                 */
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        int x = event->button.x;
        int y = event->button.y;
        if (x >= button.x && x < button.x + button.w && y >= button.y &&
            y < button.y + button.h) {
            // button clicked
            std::cout << "Button Clicked" << std::endl;
        }
    }
    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    uint64_t start = SDL_GetTicks();

    // Update

    // Chip 8 Update loop -------------
    auto instruction = chip.fetchOp();
    chip.decode(instruction);
    // --------------------------------

    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    bool hovering = (mouseX >= button.x && mouseX < button.x + button.w &&
                     mouseY >= button.y && mouseY < button.y + button.h);

    // Draw
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // background
    SDL_RenderClear(renderer);

    // Choose button color
    SDL_Color drawColor = hovering ? hoverColor : normalColor;
    if (hovering && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK) {
        drawColor = activeColor; // pressed state
    }

    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b,
                           255);
    SDL_RenderFillRect(renderer, &button);

    // Draw outline
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &button);

    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);

    // Frame Linting
    uint64_t elapsed = SDL_GetTicks() - start;
    if (elapsed < frame_time) {
        SDL_Delay((uint32_t)(frame_time - elapsed));
    }

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
}
