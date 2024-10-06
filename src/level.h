#ifndef H_LEVEL
#define H_LEVEL

#include <raylib.h>

typedef struct BoidSpawnCluster {
    int amount;
    Vector2 position;
} BoidSpawnCluster;

typedef struct Level {
    Vector2 startPos;
    Vector2 endPos; 
    int clusterCount;
    BoidSpawnCluster *spawnClusters;
    int hitboxCount;
    Rectangle *hitboxes;
} Level;

Level InitLevel(Rectangle hitboxes[], int hitboxCount, BoidSpawnCluster clusters[], int clusterCount, Vector2 startPos, Vector2 endPos);
void DrawLevel(Level* level, Texture2D marbleTex);
void FreeLevel(Level* l);

#endif
