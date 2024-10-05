#include "raylib.h"
#include "boid.h"
#include <raymath.h>
#include <stdio.h>
#include <string.h>

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

const int screenWidth = 1280;
const int screenHeight = 720;

Texture2D guyTexture;
BoidData boidData;

void UpdateDrawFrame(void);     

int main(void)
{
    InitWindow(screenWidth, screenHeight, "ld56");
    guyTexture = LoadTexture("assets/guy.png");
    boidData = InitBoidData(256);
    boidData.boidMaxSpeed = 200.0;
    boidData.boidSpeed = 1.8;
    boidData.cohesionRadius = 100;
    boidData.cohesionFactor = 0.1;
    boidData.separationRadius = 30;
    boidData.separationFactor = 10;
    boidData.alignmentRadius = 100;
    boidData.alignmentFactor = 1.0/6;
    boidData.boidRadius = 10;
    boidData.boidCount = 30;
    for(int i = 0; i < boidData.boidCount; ++i) 
    {
        boidData.boidPositions[i] = (Vector2){GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight)};
        boidData.boidVelocities[i] = Vector2Zero();
    }

    #if defined(__EMSCRIPTEN__)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
    SetTargetFPS(60);   

    while (!WindowShouldClose())    
    {
        UpdateDrawFrame();
    }
#endif

    FreeBoidData(&boidData);
    UnloadTexture(guyTexture);
    CloseWindow();        

    return 0;
}

void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    UpdateBoids(&boidData, Vector2Zero(), false, NULL);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawBoids(&boidData, guyTexture);
    EndDrawing();
}
