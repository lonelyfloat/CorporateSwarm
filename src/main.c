#include "raylib.h"
#include "boid.h"
#include <math.h>
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
    GAME_PLAYING,
    GAME_WON
} GameState;

GameState currentState = GAME_MENU;
Texture2D guyTexture;
BoidData boidData;

Texture2D playerTex;
Vector2 playerPos;
Vector2 playerVelo;
Vector2 lastFrameVelo;

Texture2D menuScreen;
Texture2D winScreen;

const float playerRadius = 20;
const float playerSpeed = 250;

const int levelCount = 3;
Level *levels;
int currentLevel;
Texture2D marbleTex;

Sound boidWalk;
Sound playerWalk;
Sound deathSound;


Music music;


void UpdateDrawFrame(void);     

int main(void)
{
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
    boidData.boidObstacleRadius = 20;
    boidData.boidConeWidth = 120;
    boidData.boidSuspicion = 2;
    boidData.suspicionRadius = 100;
    boidData.boidObjRadius = 50;

    levels = malloc(sizeof(Level) * levelCount);
    levels[0] = InitLevel(
                (Rectangle[]){
                (Rectangle){-1,0, 1, screenHeight},
                (Rectangle){0, -1, screenWidth, 1},
                (Rectangle){screenWidth+1,0, 1, screenHeight},
                (Rectangle){0, screenHeight+1, screenWidth, 1},
                (Rectangle){295, 0, 100, 350},
                (Rectangle){638, 0, 100, 320},
                (Rectangle){555, 390, 530, 165}
                },
                7,
                (BoidSpawnCluster[]){
                (BoidSpawnCluster){20, (Vector2){140, 120}},
                (BoidSpawnCluster){10, (Vector2){800,300}}
                },
                2,
                (Vector2){462, 130},
                (Vector2){1137,100}
            );
    levels[1] = InitLevel(
                (Rectangle[]){
                (Rectangle){-1,0, 1, screenHeight},
                (Rectangle){0, -1, screenWidth, 1},
                (Rectangle){screenWidth+1,0, 1, screenHeight},
                (Rectangle){0, screenHeight+1, screenWidth, 1},
                (Rectangle){392, 140, 150, 530},
                (Rectangle){700, 175, 350, 110},
                (Rectangle){700, 440, 350, 170},
                (Rectangle){700, 0, 130, 140}
                },
                8,
                (BoidSpawnCluster[]){
                (BoidSpawnCluster){35, (Vector2){850, 330}},
                },
                1,
                (Vector2){1100, 45},
                (Vector2){140,580}
            );

    levels[2] = InitLevel(
                (Rectangle[]){
                (Rectangle){0, -1, screenWidth, 1},
                (Rectangle){0, screenHeight+1, screenWidth, 1},
                (Rectangle){0, 0, 140, 300},
                (Rectangle){0, 370, 140, 350},
                (Rectangle){180, 430, 580, 70},
                (Rectangle){460, 270, 580, 70},
                (Rectangle){1140, 45, 1280-1140, 540},
                (Rectangle){1140, 645, 1280-1140, 75},
                },
                8,
                (BoidSpawnCluster[]){
                (BoidSpawnCluster){40, (Vector2){-120, 300}},
                (BoidSpawnCluster){40, (Vector2){1370,580}}
                },
                2,
                (Vector2){screenWidth/2.0, 580},
                (Vector2){screenWidth/2.0,55}
            );

    currentLevel = 0;
    LoadLevel(&boidData, &levels[currentLevel]);
    playerPos = levels[currentLevel].startPos;
    playerVelo = Vector2Zero();


    InitWindow(screenWidth, screenHeight, "ld56");
    guyTexture = LoadTexture("assets/guy.png");
    playerTex = LoadTexture("assets/player.png");
    marbleTex = LoadTexture("assets/marble.png");
    winScreen = LoadTexture("assets/winscreen.png");
    menuScreen = LoadTexture("assets/mainmenu.png");

    InitAudioDevice();
    boidWalk = LoadSound("assets/guywalk.wav");
    playerWalk = LoadSound("assets/playerwalk.wav");
    deathSound = LoadSound("assets/death.wav");
    SetSoundVolume(playerWalk, 1.0);
    SetSoundVolume(boidWalk, 1.0);

    music = LoadMusicStream("assets/maintheme.wav");

    PlayMusicStream(music);

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
    UnloadSound(boidWalk);
    free(levels);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();        

    return 0;
}

void UpdateDrawFrame(void)
{

    UpdateMusicStream(music);
    switch(currentState) 
    {
    case GAME_PLAYING:
    // Update
    //----------------------------------------------------------------------------------
    // TODO: b-section of music, adaptive music stuff
    lastFrameVelo = playerVelo;
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        playerVelo = Vector2Scale(Vector2Normalize(Vector2Subtract(GetMousePosition(), playerPos)), playerSpeed * GetFrameTime());
    else playerVelo = Vector2Zero();
    if(Vector2LengthSqr(playerVelo) > 0.3 && !IsSoundPlaying(playerWalk)) PlaySound(playerWalk);
    if(Vector2Equals(playerVelo, Vector2Zero())) StopSound(playerWalk);
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
    int soundCount = 0;

    if(!IsSoundPlaying(boidWalk)) 
    {
        PlaySound(boidWalk);
    }
    for(int i = 0; i < boidData.boidCount; ++i)
    {
        if(Vector2Length(boidData.boidVelocities[i]) > boidData.boidSpeed*1.5) soundCount++;
        if(CheckCollisionCircles(playerPos, playerRadius, boidData.boidPositions[i], boidData.boidRadius))
        {

            LoadLevel(&boidData, &levels[currentLevel]);
            playerPos = levels[currentLevel].startPos;
            PlaySound(deathSound);
            break;
        }
    }

    SetSoundVolume(boidWalk, sqrtf(sqrt(soundCount/(float)boidData.boidCount)));


    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground((Color){188,188,188,255});
        DrawBoids(&boidData, guyTexture);
        DrawLevel(&levels[currentLevel], marbleTex);
        DrawTexturePro(playerTex, (Rectangle){0,0,playerTex.width,playerTex.height}, 
            (Rectangle){playerPos.x, playerPos.y, playerTex.width, playerTex.height},
            (Vector2){playerTex.width/2.0, playerTex.height/2.0}, 30*sinf(10*GetTime()), WHITE);
    EndDrawing();

    if(CheckCollisionCircles(playerPos, playerRadius, levels[currentLevel].endPos, marbleTex.width/2.0))
    {
        currentLevel++;
        if(currentLevel >= levelCount)
        {
            currentState = GAME_WON;
        }
        else 
        {
            LoadLevel(&boidData, &levels[currentLevel]);
            playerPos = levels[currentLevel].startPos;
        }
    }
    break;
    case GAME_MENU: 
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        currentState = GAME_PLAYING;
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawTexture(menuScreen, 0, 0, WHITE);
    EndDrawing();
    break;


    case GAME_WON:
    if(IsSoundPlaying(playerWalk)) StopSound(playerWalk);
    if(IsSoundPlaying(boidWalk)) StopSound(boidWalk);
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {

        currentLevel = 0;
        LoadLevel(&boidData, &levels[currentLevel]);
        playerPos = levels[currentLevel].startPos;
        currentState = GAME_PLAYING;
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawTexture(winScreen, 0,0, WHITE);
    EndDrawing();
    break;
    }
}
