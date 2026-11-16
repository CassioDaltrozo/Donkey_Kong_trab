#include "animations.h"
#include "raylib.h"

// UPTADE DE ANIMAÇAO, USANDO PONTEIROS
void animation_update(Animation *self)
{
    float delta_time = GetFrameTime();
    self->duration_left -= delta_time;

    if (self->duration_left <= 0.0)
    {
        self->duration_left = self->speed;

        if (self->current_frame == self->last_frame && self->type == ONESHOT)
        {
            return;
        }

        self->current_frame += self->step;

        if (self->current_frame > self->last_frame)
        {
            switch (self->type)
            {
            case REPEATING:
                self->current_frame = self->first_frame;
                break;

            case ONESHOT:
                self->current_frame = self->last_frame;
                break;
            }
        }
        else if (self->current_frame < self->first_frame)
        {
            switch (self->type)
            {
            case REPEATING:
                self->current_frame = self->last_frame;
                break;

            case ONESHOT:
                self->current_frame = self->first_frame;
                break;
            }
        }
    }
}

// RETORNO DO FRAME DA PNG PARA DESENHO
Rectangle animation_frame(Animation *self, int num_frames_per_row)
{
    float x = (float)(self->current_frame % num_frames_per_row) * 256.0f;
    float y = (float)(self->current_frame / num_frames_per_row) * 256.0f;

    return (Rectangle){.x = x, .y = y, .width = 256.0f, .height = 256.0f};
}

// RESET PARA PERMANENCIA
void reset_animation(Animation *self)
{
    self->current_frame = self->first_frame;
    self->duration_left = self->speed;
}