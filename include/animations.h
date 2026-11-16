#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include "raylib.h"

typedef enum Direction
{
    LEFT = -1,
    RIGHT = 1,
} Direction;

typedef enum PlayerState
{
    IDLE,
    RUNNING,
    JUMPING,
    ATTACKING
} PlayerState;

typedef enum AnimationType
{
    REPEATING = 1,
    ONESHOT = 2,
} AnimationType;

typedef struct Animation
{
    int first_frame;
    int last_frame;
    int current_frame;
    int step;
    float speed;
    float duration_left;
    AnimationType type;
} Animation;


// ASSINA FUNCOES DE ANIMACAO DE SPRITE
void animation_update(Animation *self);
void reset_animation(Animation *self);
Rectangle animation_frame(Animation *self, int num_frames_per_row);

#endif