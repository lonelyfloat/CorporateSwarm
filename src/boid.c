#include "boid.h"
#include "raylib.h"
#include "stdlib.h"
#include <raymath.h>

#define ROTATE_FREQ 8 
BoidData InitBoidData(int maxBoids)
{
    BoidData b;
    b.boidPositions = calloc(maxBoids, sizeof(Vector2));
    b.boidVelocities = calloc(maxBoids, sizeof(Vector2));
    b.maxBoids = maxBoids;
    return b;
}

void UpdateBoids(BoidData* b, Vector2 playerPos, bool boidState, Level* l)
{
    for(int i = 0; i < b->boidCount; ++i)
    {
        Vector2 lastFrameVelo = b->boidVelocities[i];
        // Rule 1: Cohesion

        Vector2 positionsSum = Vector2Zero();
        int matches = 0;
        for(int j = 0; j < b->boidCount; ++j)
        {
            if(i != j && CheckCollisionPointCircle(b->boidPositions[j], b->boidPositions[i], b->cohesionRadius))
            {
                positionsSum = Vector2Add(positionsSum, b->boidPositions[j]);
                ++matches;
            }
        }
        Vector2 centerOfMass;
        if(matches != 0) centerOfMass = Vector2Scale(positionsSum, 1.0/matches);
        else centerOfMass = b->boidPositions[i]; 
        b->boidVelocities[i] = Vector2Scale(Vector2Subtract(centerOfMass, b->boidPositions[i]), b->cohesionFactor);

        if(boidState) b->boidVelocities[i] = Vector2Add(b->boidVelocities[i], Vector2Scale(Vector2Subtract(playerPos, b->boidPositions[i]), 0.3));

        // Rule 2: Separation
        Vector2 dir = Vector2Zero();
        for(int j = 0; j < b->boidCount; ++j) 
        {
            if(j != i && CheckCollisionPointCircle(b->boidPositions[j], b->boidPositions[i], b->separationRadius))
            {
                dir = Vector2Add(dir, Vector2Subtract(b->boidPositions[i], b->boidPositions[j]));
            }
        }
        /* LEVEL STUFF
        for(int h = 0; h < l->hitboxCount; ++h)
        {
            bool verticalNormal = CheckCollisionCircleRec(b->boidPositions[i], b->separationRadius, (Rectangle){l->hitbox[h].x, l->hitbox[h].y, l->hitbox[h].width,1}) ||
             CheckCollisionCircleRec(b->boidPositions[i], b->separationRadius, (Rectangle){l->hitbox[h].x, l->hitbox[h].y+l->hitbox[h].height, l->hitbox[h].width,1});
            bool horizontalNormal = CheckCollisionCircleRec(b->boidPositions[i], b->separationRadius, (Rectangle){l->hitbox[h].x, l->hitbox[h].y, 1, l->hitbox[h].height}) ||
             CheckCollisionCircleRec(b->boidPositions[i], b->separationRadius, (Rectangle){l->hitbox[h].x+l->hitbox[h].width, l->hitbox[h].y, 1,l->hitbox[h].height});
            if(verticalNormal) dir = Vector2Add(dir, (Vector2){dir.x, -dir.y});
            if(verticalNormal) dir = Vector2Add(dir, (Vector2){+dir.x, dir.y});
        }
        */
        b->boidVelocities[i] = Vector2Add(b->boidVelocities[i], Vector2Scale(dir, b->separationFactor));

        // Rule 3: Alignment

        Vector2 averageVelo = Vector2Zero();
        int alignmentMatches = 0;
        for(int j = 0; j < b->boidCount; ++j) 
        {
            if(j != i && CheckCollisionPointCircle(b->boidVelocities[j], b->boidVelocities[i], b->alignmentRadius)) 
            {
                averageVelo = Vector2Add(averageVelo, b->boidVelocities[j]);
            }
        }
        if(alignmentMatches != 0) averageVelo = Vector2Scale(averageVelo, 1.0/alignmentMatches);
        b->boidVelocities[i] = Vector2Add(b->boidVelocities[i], Vector2Scale(averageVelo, b->alignmentFactor));

        // Velocity application
        // b->boidVelocities[i] = Vector2Scale(Vector2Normalize(b->boidVelocities[i]), b->boidSpeed * GetFrameTime());
        b->boidVelocities[i] = Vector2Scale(b->boidVelocities[i], b->boidSpeed * GetFrameTime());
        if(Vector2LengthSqr(b->boidVelocities[i]) > (b->boidMaxSpeed*b->boidMaxSpeed) * GetFrameTime()) 
            b->boidVelocities[i] = Vector2Scale(Vector2Normalize(b->boidVelocities[i]), b->boidMaxSpeed * GetFrameTime()); 
        float lerpf = 0.2; // lerp value for linear interpolation of velocity (below):
        b->boidVelocities[i].x = lastFrameVelo.x * (1.0 - lerpf) + (b->boidVelocities[i].x * lerpf);
        b->boidVelocities[i].y = lastFrameVelo.y * (1.0 - lerpf) + (b->boidVelocities[i].y * lerpf);
        b->boidPositions[i] = Vector2Add(b->boidPositions[i], b->boidVelocities[i]);
    }
}


void DrawBoids(BoidData* b, Texture2D bt)
{
    for(int i = 0; i < b->boidCount; ++i)
    { 
        DrawTexturePro(bt, (Rectangle){0,0,bt.width, bt.height}, 
            (Rectangle){b->boidPositions[i].x, b->boidPositions[i].y, bt.width, bt.height},
            (Vector2){bt.width/2., bt.height/2.}, 30*sinf((GetTime() + i) * ROTATE_FREQ), WHITE);
    }
}

void FreeBoidData(BoidData* b)
{
    free(b->boidPositions);
    free(b->boidVelocities);
}
