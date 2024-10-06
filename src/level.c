#include "level.h"
#include <stdlib.h>
#include <raymath.h>

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

void DrawLevel(Level* l, Texture2D marbleTex)
{
    for(int i = 0; i < l->hitboxCount; ++i) 
    {
        DrawRectangleRec(l->hitboxes[i], GRAY);
        DrawRectangleLinesEx(l->hitboxes[i], 4.0, BLACK);
    }
    DrawTexturePro(marbleTex, (Rectangle){0,0,marbleTex.width, marbleTex.height}, (Rectangle){l->endPos.x, l->endPos.y, marbleTex.width, marbleTex.height},
            (Vector2){marbleTex.width/2.0, marbleTex.height/2.0}, 0, WHITE);
}

void FreeLevel(Level* l)
{
    free(l->hitboxes);
    free(l->spawnClusters);
}
