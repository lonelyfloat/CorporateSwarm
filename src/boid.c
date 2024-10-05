#include "boid.h"
#include "raylib.h"
#include "stdlib.h"
#include <raymath.h>
#include <stdio.h>

#define ROTATE_FREQ 8 
BoidData InitBoidData(int maxBoids)
{
    BoidData b;
    b.boidPositions = calloc(maxBoids, sizeof(Vector2));
    b.boidVelocities = calloc(maxBoids, sizeof(Vector2));
    b.boidStates = calloc(maxBoids, sizeof(BoidState));
    b.boidTimers = calloc(maxBoids, sizeof(float));
    for(int i = 0; i < maxBoids; ++i)
    {
        b.boidTimers[i] = -1;
        b.boidVelocities[i] = Vector2Zero();
        b.boidStates[i] = BOID_IDLE;
    }
    b.maxBoids = maxBoids;

    return b;
}

void LoadLevel(BoidData* b, Level* l)
{
    int totalSize = 0;
    for(int i = 0; i < l->clusterCount; ++i)
    {
        totalSize += l->spawnClusters[i].amount;
    }
    b->boidCount = totalSize;
    if(totalSize > b->maxBoids) printf("ERROR: TOO MANY BOIDS IN LEVEL\n");
    int currentCluster = 0;
    int clusterIdx = 0;
    for(int i = 0; i < b->boidCount; ++i)
    {
        b->boidPositions[i] = l->spawnClusters[currentCluster].position;
        clusterIdx++;
        if(clusterIdx >= l->spawnClusters[currentCluster].amount) 
        {
            clusterIdx = 0;
            currentCluster++;
        }
    }
}

void UpdateBoids(BoidData* b, Vector2 playerPos, Level* l)
{
    for(int i = 0; i < b->boidCount; ++i)
    {
        if(b->boidStates[i] != BOID_IDLE) b->boidTimers[i] -= GetFrameTime();

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

        if(b->boidStates[i] != BOID_IDLE ) b->boidVelocities[i] = Vector2Add(b->boidVelocities[i], Vector2Scale(Vector2Subtract(playerPos, b->boidPositions[i]), 0.3));

        // Obstacle avoidance (aboidance)
        for(int k = -b->boidConeWidth/2; k < b->boidConeWidth/2; ++k)
        {
            bool rayColliding = false;
            Vector2 forward = Vector2Scale(Vector2Rotate(Vector2Normalize(b->boidVelocities[i]), k), b->boidObstacleRadius);
            for(int h = 0; h < l->hitboxCount; ++h)
            {
                Rectangle r = l->hitboxes[h];
                Vector2 nearestIntersectionPt;
                Vector2 intersectPtA = Vector2Zero();
                if(CheckCollisionLines(b->boidPositions[i], Vector2Add(b->boidPositions[i], forward),
                                            (Vector2){r.x, r.y}, (Vector2){r.x + r.width, r.y}, &intersectPtA)) 
                {
                    rayColliding = true;
                    nearestIntersectionPt = intersectPtA;
                }

                Vector2 intersectPtB = Vector2Zero();
                if(CheckCollisionLines(b->boidPositions[i], Vector2Add(b->boidPositions[i], forward),
                            (Vector2){r.x, r.y}, (Vector2){r.x, r.y + r.height}, &intersectPtB))
                {
                    rayColliding = true;
                    if(Vector2Distance(b->boidPositions[i], intersectPtB) < Vector2Distance(b->boidPositions[i], nearestIntersectionPt))
                        nearestIntersectionPt = intersectPtB;
                }

                Vector2 intersectPtC = Vector2Zero();
                if(CheckCollisionLines(b->boidPositions[i], Vector2Add(b->boidPositions[i], forward),
                            (Vector2){r.x + r.width, r.y}, (Vector2){r.x + r.width, r.y + r.height}, &intersectPtC)) 
                {
                    rayColliding = true;
                    if(Vector2Distance(b->boidPositions[i], intersectPtC) < Vector2Distance(b->boidPositions[i], nearestIntersectionPt))
                        nearestIntersectionPt = intersectPtC;
                }

                Vector2 intersectPtD = Vector2Zero();
                if(CheckCollisionLines(b->boidPositions[i], Vector2Add(b->boidPositions[i], forward),
                            (Vector2){r.x, r.y + r.height}, (Vector2){r.x + r.width, r.y + r.height}, &intersectPtD)) 
                {
                    rayColliding = true;
                    if(Vector2Distance(b->boidPositions[i], intersectPtD) < Vector2Distance(b->boidPositions[i], nearestIntersectionPt))
                        nearestIntersectionPt = intersectPtD;
                }
                if(rayColliding)
                {
                    Vector2 avoidDir = Vector2Negate(Vector2Subtract(nearestIntersectionPt, b->boidPositions[i]));
                    b->boidVelocities[i] = Vector2Add(b->boidVelocities[i], Vector2Scale(avoidDir, b->boidObstacleFactor));
                }
            }
        }

        // Rule 2: Separation
        Vector2 dir = Vector2Zero();

        int suspiciousNeighbors = 0;
        for(int j = 0; j < b->boidCount; ++j) 
        {
            if(j != i && CheckCollisionPointCircle(b->boidPositions[j], b->boidPositions[i], b->separationRadius))
                dir = Vector2Add(dir, Vector2Subtract(b->boidPositions[i], b->boidPositions[j]));
            if(j != i && CheckCollisionPointCircle(b->boidPositions[j], b->boidPositions[i], b->suspicionRadius))
                if(b->boidStates[j] == BOID_CHASE) suspiciousNeighbors++;
        }
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
        b->boidVelocities[i] = Vector2Scale(b->boidVelocities[i], b->boidSpeed * GetFrameTime());
        if(Vector2LengthSqr(b->boidVelocities[i]) > (b->boidMaxSpeed*b->boidMaxSpeed) * GetFrameTime()) 
            b->boidVelocities[i] = Vector2Scale(Vector2Normalize(b->boidVelocities[i]), b->boidMaxSpeed * GetFrameTime()); 
        b->boidVelocities[i] = Vector2Lerp(lastFrameVelo, b->boidVelocities[i], 0.2);
        b->boidPositions[i] = Vector2Add(b->boidPositions[i], b->boidVelocities[i]);

        bool obscured = false;
        for(int h = 0; h < l->hitboxCount; h++)
        {
            Rectangle r = l->hitboxes[h];
            obscured = obscured || CheckCollisionLines(b->boidPositions[i],playerPos,(Vector2){r.x, r.y},(Vector2){r.x + r.width, r.y}, NULL)||
                       CheckCollisionLines(b->boidPositions[i],playerPos,(Vector2){r.x, r.y},(Vector2){r.x, r.y + r.height}, NULL)||
                       CheckCollisionLines(b->boidPositions[i],playerPos,(Vector2){r.x + r.width, r.y},(Vector2){r.x + r.width, r.y + r.height}, NULL)||
                       CheckCollisionLines(b->boidPositions[i],playerPos,(Vector2){r.x, r.y + r.height},(Vector2){r.x + r.width, r.y + r.height}, NULL);
            if(i==0 && h == 0)printf("%d: %s\n", i, obscured ? "true" : "false");
        }

        if(!obscured)
        {
            b->boidStates[i] = BOID_CHASE;
            b->boidTimers[i] = b->boidForgetTime;
        }
        if(suspiciousNeighbors > b->boidSuspicion)
        {
            b->boidStates[i] = BOID_FOLLOW;
            b->boidTimers[i] = b->boidForgetTime;
        }
        if(b->boidTimers[i] <= 0) 
        {
            b->boidTimers[i] = -1;
            b->boidStates[i] = BOID_IDLE;
            if(b->boidTimers[i] != -1) b->boidVelocities[i] = Vector2Scale(Vector2Normalize((Vector2){GetRandomValue(-1,1), GetRandomValue(-1,1)}), b->boidMaxSpeed*GetFrameTime());
        }
    }
}


void DrawBoids(BoidData* b, Texture2D bt)
{
    for(int i = 0; i < b->boidCount; ++i)
    { 
        DrawTexturePro(bt, (Rectangle){0,0,bt.width, bt.height}, 
            (Rectangle){b->boidPositions[i].x, b->boidPositions[i].y, bt.width, bt.height},
            (Vector2){bt.width/2., bt.height/2.},(30*sinf((GetTime() + i) * ROTATE_FREQ)), WHITE);
    }
}

void FreeBoidData(BoidData* b)
{
    free(b->boidPositions);
    free(b->boidVelocities);
    free(b->boidStates);
    free(b->boidTimers);
}
