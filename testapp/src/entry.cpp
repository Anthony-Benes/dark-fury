#include <entry.hpp>
#include "game.hpp"

#include <platform/platform.hpp>

b8 create_game(game* out_game) {
    out_game->app_config = { 100, 100, 1280, 720, "Dark Fury Test App" };
    out_game->initialize = game_initialize;
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->on_resize = game_on_resize;
    out_game->state = platform_allocate(sizeof(game_state), FALSE);
    return TRUE;
}
