#include "raylib.h"
#include "boid.h"
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

const int screenWidth = 1280;
const int screenHeight = 720;

typedef enum GameState
{
    GAME_MENU,
    GAME_DIED,
    GAME_PLAYING,
    GAME_WON
} GameState;
Texture2D guyTexture;
BoidData boidData;

Texture2D playerTex;
Vector2 playerPos;
Vector2 playerVelo;

const float playerRadius = 20;
const float playerSpeed = 250;

const int levelCount = 1;
Level *levels;
int currentLevel;
Texture2D marbleTex;

void UpdateDrawFrame(void);     

int main(void)
{
    playerPos = (Vector2){screenWidth/2.0, screenHeight/2.0};
    playerVelo = Vector2Zero();
    InitWindow(screenWidth, screenHeight, "ld56");
    guyTexture = LoadTexture("assets/guy.png");
    playerTex = LoadTexture("assets/player.png");
    marbleTex = LoadTexture("assets/marble.png");

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
    boidData.boidObjRadius = 50;

    levels = malloc(sizeof(Level) * levelCount);
    levels[0] = InitLevel(
                (Rectangle[]){
                (Rectangle){100,100, 300, 200},
                (Rectangle){500,100, 300, 200}
                },
                3,
                (BoidSpawnCluster[]){(BoidSpawnCluster){30, (Vector2){100, 600}}},
                1,
                (Vector2){screenWidth/2.0, screenHeight/2.0},
                (Vector2){800,400}
            );
    currentLevel = 0;
    LoadLevel(&boidData, &levels[currentLevel]);



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
    for(int i = 0; i < levelCount; ++i)
    {
        FreeLevel(&levels[i]);
    }
    UnloadTexture(guyTexture);
    UnloadTexture(playerTex);
    UnloadTexture(marbleTex);
    free(levels);
    CloseWindow();        

    return 0;
}

void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Menus, sounds, b-section of music, levels, finish level state machine type stuff (the big switch)
    Vector2 lastFrameVelo = playerVelo;
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        playerVelo = Vector2Scale(Vector2Normalize(Vector2Subtract(GetMousePosition(), playerPos)), playerSpeed * GetFrameTime());
    else playerVelo = Vector2Zero();
    playerVelo = Vector2Lerp(lastFrameVelo, playerVelo, 0.3);

    bool collidingwHitbox = false;
    for(int i = 0; i < levels[currentLevel].hitboxCount; ++i)
    {
        if(CheckCollisionCircleRec(Vector2Add(playerPos, Vector2Scale(playerVelo, 3)), playerRadius, levels[currentLevel].hitboxes[i]))
        {
            playerPos = Vector2Add(playerPos, Vector2Negate(playerVelo));
            collidingwHitbox = true;
            break;
        }
    }
    //if(collidingwHitbox) playerVelo = Vector2Zero();
    playerPos = Vector2Add(playerPos, playerVelo);
    UpdateBoids(&boidData, playerPos, &levels[currentLevel]);
    bool playerBoidCollision = false;
    for(int i = 0; i < boidData.boidCount; ++i)
    {
        if(CheckCollisionCircles(playerPos, playerRadius, boidData.boidPositions[i], boidData.boidRadius))
        {
            playerBoidCollision = true;
            break;
        }
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawBoids(&boidData, guyTexture);
        DrawLevel(&levels[currentLevel], marbleTex);
        DrawTexturePro(playerTex, (Rectangle){0,0,playerTex.width,playerTex.height}, 
            (Rectangle){playerPos.x, playerPos.y, playerTex.width, playerTex.height},
            (Vector2){playerTex.width/2.0, playerTex.height/2.0}, 30*sinf(10*GetTime()), WHITE);
    EndDrawing();
}
