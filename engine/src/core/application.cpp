#include <core/application.hpp>

#include <game_types.hpp>
#include <core/logger.hpp>
#include <core/df_memory.hpp>
#include <platform/platform.hpp>

typedef struct application_state {
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
} application_state;

static b8 initialized = FALSE;
static application_state app_state;

b8 application_create(game* game_inst) {
    if (initialized) {
        DF_ERROR("application_create called more than once.");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize Sub-systems
    initialize_logging();

    DF_FATAL("Test format %f", 3.14f);
    DF_ERROR("Test format %f", 3.14f);
    DF_WARN("Test format %f", 3.14f);
    DF_INFO("Test format %f", 3.14f);
    DF_DEBUG("Test format %f", 3.14f);
    DF_TRACE("Test format %f", 3.14f);

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    auto config = app_state.game_inst->app_config;
    if (!platform_startup(&app_state.platform, config.name, config.start_pos_x,
                          config.start_pos_y, config.start_pos_width,
                          config.start_pos_height)) {
        return FALSE;
    }
    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        DF_FATAL("Game failed to initialize.");
        return FALSE;
    }
    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);
    initialized = TRUE;
    return TRUE;
}

b8 application_run() {
    DF_INFO(get_memory_usage_str());
    while (app_state.is_running) {
        if (!platform_pump_message(&app_state.platform)) {
            app_state.is_running = FALSE;
        }
        if (!app_state.is_suspended) {
            if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
                DF_FATAL("Game update failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }
            if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
                DF_FATAL("Game render failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }
        }
    }
    app_state.is_running = FALSE;
    platform_shutdown(&app_state.platform);
    return TRUE;
}
