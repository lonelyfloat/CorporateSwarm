#ifndef H_BOID
#define H_BOID

#include <raylib.h>
#include "level.h"

typedef struct BoidData {

    Vector2 *boidPositions;
    Vector2 *boidVelocities;
    int maxBoids;
    int boidCount;
    float boidMaxSpeed;
    float boidSpeed;
    float cohesionRadius;
    float cohesionFactor;
    float separationRadius;
    float separationFactor;
    float alignmentRadius;
    float alignmentFactor;
    float boidRadius;
} BoidData;

BoidData InitBoidData(int maxBoids);
void UpdateBoids(BoidData* b, Vector2 playerPos, bool boidState, Level* l);
void DrawBoids(BoidData* b, Texture2D bt);
void FreeBoidData(BoidData* b);

#endif
