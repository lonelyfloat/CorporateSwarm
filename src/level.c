#include "level.h"
#include <stdlib.h>

Level InitLevel(Rectangle hitboxes[], int hitboxCount, BoidSpawnCluster clusters[], int clusterCount, Vector2 startPos, Vector2 endPos)
{
    Level l;
    l.hitboxCount = hitboxCount;
    l.clusterCount = clusterCount;
    l.startPos = startPos;
    l.endPos = endPos;
    l.hitboxes = calloc(hitboxCount, sizeof(Rectangle));
    for(int i = 0; i < hitboxCount; ++i)
    {
        l.hitboxes[i] = hitboxes[i];
    }
    l.spawnClusters = calloc(clusterCount, sizeof(BoidSpawnCluster));
    for(int i = 0; i < clusterCount; ++i)
    {
        l.spawnClusters[i] = clusters[i];
    }

    return l;
}

void DrawLevel(Level* l)
{
    for(int i = 0; i < l->hitboxCount; ++i) 
    {
        DrawRectangleRec(l->hitboxes[i], GRAY);
    }
}

void FreeLevel(Level* l)
{
    free(l->hitboxes);
    free(l->spawnClusters);
}
