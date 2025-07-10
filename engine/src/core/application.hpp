#pragma once

#include <defines.hpp>

struct game;

typedef struct application_config {
    i16 start_pos_x;
    i16 start_pos_y;
    i16 start_pos_width;
    i16 start_pos_height;
    const char* name;
} application_config;

DF_API b8 application_create(struct game* game_inst);

DF_API b8 application_run();
