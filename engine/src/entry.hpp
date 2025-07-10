#pragma once

#include <core/application.hpp>
#include <core/logger.hpp>
#include <core/df_memory.hpp>
#include <game_types.hpp>

extern b8 create_game(game* out_game);

int main(void) {
    initialize_memory();
    game game_inst;
    if (!create_game(&game_inst)) {
        DF_FATAL("Could not create game!");
        return -1;
    }
    if (!game_inst.render || !game_inst.update || !game_inst.initialize || !game_inst.on_resize) {
        DF_FATAL("The game's function pointers must be assigned!");
        return -2;
    }
    if (!application_create(&game_inst)) {
        DF_INFO("Application failed to create!");
        return 1;
    }
    if (!application_run()) {
        DF_INFO("Application did not shutdown gracefully.");
        return 2;
    }
    shutdown_memory();
    return 0;
}
