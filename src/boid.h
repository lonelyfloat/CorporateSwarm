#ifndef H_BOID
#define H_BOID

#include <raylib.h>
#include "level.h"

typedef enum BoidState {
    BOID_IDLE,
    BOID_CHASE, // actively sees
    BOID_FOLLOW // following crowd of suspicion
} BoidState; // Todo: do view cones in UpdateBoids

typedef struct BoidData {
    Vector2 *boidPositions;
    Vector2 *boidVelocities;
    BoidState* boidStates;
    float* boidTimers;
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
    float boidViewTolerance;
    float boidConeWidth;
    float boidForgetTime;
    float boidObstacleFactor;
    float boidObstacleRadius;
    float suspicionRadius;
    int boidSuspicion;
} BoidData;

BoidData InitBoidData(int maxBoids);
void LoadLevel(BoidData* b, Level* l);
void UpdateBoids(BoidData* b, Vector2 playerPos, Level* l);
void DrawBoids(BoidData* b, Texture2D bt);
void FreeBoidData(BoidData* b);

#endif
