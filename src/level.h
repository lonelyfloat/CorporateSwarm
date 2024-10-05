#ifndef H_LEVEL
#define H_LEVEL

#include <raylib.h>

typedef struct Level {
    Vector2* boidStartingPos;
    int boidCount;
    Rectangle* hitbox;
    int hitboxCount;
    Vector2 startPos;
    Vector2 endGoal;
} Level;


#endif
