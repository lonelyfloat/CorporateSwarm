#include "raylib.h"
#include "boid.h"
#include <raymath.h>

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

const int screenWidth = 1280;
const int screenHeight = 720;

Texture2D guyTexture;
BoidData boidData;

Vector2 playerPos;
Vector2 playerVelo;
const float playerRadius = 20;
const float playerSpeed = 250;
Level level1;

void UpdateDrawFrame(void);     

int main(void)
{
    playerPos = (Vector2){screenWidth/2.0, screenHeight/2.0};
    playerVelo = Vector2Zero();
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
    boidData.boidViewTolerance = 0.85;
    boidData.boidForgetTime = 2.0;
    boidData.boidObstacleFactor = 1.0;
    boidData.boidObstacleRadius = 30;
    boidData.boidConeWidth = 120;
    boidData.boidSuspicion = 2;
    boidData.suspicionRadius = 100;

    level1 = InitLevel(
                (Rectangle[]){
                (Rectangle){100,100, 300, 200},
                (Rectangle){500,100, 300, 200}
                },
                2,
                (BoidSpawnCluster[]){(BoidSpawnCluster){30, (Vector2){100, 600}}},
                1,
                (Vector2){screenWidth/2.0, screenHeight/2.0},
                Vector2Zero()
            );
    LoadLevel(&boidData, &level1);



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
    FreeLevel(&level1);
    UnloadTexture(guyTexture);
    CloseWindow();        

    return 0;
}

void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    Vector2 lastFrameVelo = playerVelo;
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        playerVelo = Vector2Scale(Vector2Normalize(Vector2Subtract(GetMousePosition(), playerPos)), playerSpeed * GetFrameTime());
    else playerVelo = Vector2Zero();
    playerVelo = Vector2Lerp(lastFrameVelo, playerVelo, 0.3);
    playerPos = Vector2Add(playerPos, playerVelo);
    UpdateBoids(&boidData, playerPos, &level1);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawBoids(&boidData, guyTexture);
        DrawLevel(&level1);
        DrawCircleV(playerPos, playerRadius, GREEN);
    EndDrawing();
}
