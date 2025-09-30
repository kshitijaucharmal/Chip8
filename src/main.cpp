#include <raylib.h>

int main() {
    int screenwidth = 64;
    int screenheight = 32;

    int tilesize = 8;

    InitWindow(screenwidth * tilesize, screenheight * tilesize, "Chip8");

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Ready", 100, 100, 18, BLACK);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
