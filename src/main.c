#include "raylib.h"
#include <stdio.h>
#include <raymath.h>

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

const int screenWidth = 800;
const int screenHeight = 450;
#define BOID_MAX 256

Vector2 boidPositions[BOID_MAX] = {};
Vector2 boidVelocities[BOID_MAX] = {};
float boidMaxSpeed = 200.0;
float boidSpeed = 1.8;
float cohesionRadius = 100;
float cohesionFactor = 0.001;
float separationRadius = 30;
float separationFactor = 5;
float alignmentRadius = 100;
float alignmentFactor = 1.0/6;
float boidRadius = 10;
int boidCount = 20;

void UpdateDrawFrame(void);     

int main(void)
{
    InitWindow(screenWidth, screenHeight, "ld56");
    for(int i = 0; i < boidCount; ++i) 
    {
        boidPositions[i] = (Vector2){GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight)};
        boidVelocities[i] = Vector2Zero();
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

    CloseWindow();        

    return 0;
}

void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------

    // Update boids
    for(int i = 0; i < boidCount; ++i)
    {
        // Rule 1: Cohesion

        Vector2 positionsSum = Vector2Zero();
        int matches = 0;
        for(int j = 0; j < boidCount; ++j)
        {
            if(i != j && CheckCollisionPointCircle(boidPositions[j], boidPositions[i], cohesionRadius))
            {
                positionsSum = Vector2Add(positionsSum, boidPositions[j]);
                ++matches;
            }
        }
        Vector2 centerOfMass;
        if(matches != 0) centerOfMass = Vector2Scale(positionsSum, 1.0/matches);
        else centerOfMass = boidPositions[i]; 
        boidVelocities[i] = Vector2Scale(Vector2Subtract(centerOfMass, boidPositions[i]), cohesionFactor);

        // Move towards mouse cursor 
        boidVelocities[i] = Vector2Add(boidVelocities[i], Vector2Scale(Vector2Subtract(GetMousePosition(), boidPositions[i]), 0.4));

        // Rule 2: Separation
        Vector2 dir = Vector2Zero();
        for(int j = 0; j < boidCount; ++j) 
        {
            if(j != i && CheckCollisionPointCircle(boidPositions[j], boidPositions[i], separationRadius))
            {
                dir = Vector2Add(dir, Vector2Subtract(boidPositions[i], boidPositions[j]));
            }
        }
        boidVelocities[i] = Vector2Add(boidVelocities[i], Vector2Scale(dir, separationFactor));

        // Rule 3: Alignment

        Vector2 averageVelo = Vector2Zero();
        int alignmentMatches = 0;
        for(int j = 0; j < boidCount; ++j) 
        {
            if(j != i && CheckCollisionPointCircle(boidVelocities[j], boidVelocities[i], alignmentRadius)) 
            {
                averageVelo = Vector2Add(averageVelo, boidVelocities[j]);
            }
        }
        if(alignmentMatches != 0) averageVelo = Vector2Scale(averageVelo, 1.0/alignmentMatches);
        boidVelocities[i] = Vector2Add(boidVelocities[i], Vector2Scale(averageVelo, alignmentFactor));

        // Velocity application
        // boidVelocities[i] = Vector2Scale(Vector2Normalize(boidVelocities[i]), boidSpeed * GetFrameTime());
        boidVelocities[i] = Vector2Scale(boidVelocities[i], boidSpeed * GetFrameTime());
        if(Vector2LengthSqr(boidVelocities[i]) > (boidMaxSpeed*boidMaxSpeed) * GetFrameTime()) 
            boidVelocities[i] = Vector2Scale(Vector2Normalize(boidVelocities[i]), boidMaxSpeed * GetFrameTime()); 
        boidPositions[i] = Vector2Add(boidPositions[i], boidVelocities[i]);
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        for(int i = 0; i < boidCount; ++i)
        { 
            DrawCircleV(boidPositions[i], boidRadius, RED);
        }
    EndDrawing();
}
