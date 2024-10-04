#include "raylib.h"

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

const int screenWidth = 800;
const int screenHeight = 450;

void UpdateDrawFrame(void);     

int main(void)
{
    InitWindow(screenWidth, screenHeight, "ld56");

    #if defined(__EMSCRIPTEN__)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
    SetTargetFPS(60);   

    while (!WindowShouldClose())    
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();        

    return 0;
}

void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

    EndDrawing();
}
