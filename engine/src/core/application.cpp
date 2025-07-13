#include <core/application.hpp>

#include <game_types.hpp>
#include <core/df_memory.hpp>
#include <core/event.hpp>
#include <core/input.hpp>
#include <core/logger.hpp>
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

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

b8 application_create(game* game_inst) {
    if (initialized) {
        DF_ERROR("application_create called more than once.");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize Sub-systems
    initialize_logging();
    Input::Initialize();

    DF_FATAL("Test format %f", 3.14f);
    DF_ERROR("Test format %f", 3.14f);
    DF_WARN("Test format %f", 3.14f);
    DF_INFO("Test format %f", 3.14f);
    DF_DEBUG("Test format %f", 3.14f);
    DF_TRACE("Test format %f", 3.14f);

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if (!event_initialize()) {
        DF_ERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

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

            // Input update/state copying should always be handled after any input
            // should be recorded; I.E. before this line. As a safety, input is the
            // last thing to be updated before this frame ends.
            Input::Update(0);
        }
    }
    app_state.is_running = FALSE;
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_shutdown();
    Input::Shutdown();
    platform_shutdown(&app_state.platform);
    return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            DF_INFO("EVENT_CODE_APPLICATION_QUIT received, shutting down.\n");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_KEY_PRESSED: {
            u16 key_code = context.data.d_u16[0];
            if (key_code == Input::Keys::Escape) {
                event_context data = {};
                event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
                return TRUE;
            } else if (key_code == Input::Keys::A) {
                DF_DEBUG("Explicit - A key pressed!");
            } else {
                DF_DEBUG("'%c' key pressed in a window.", key_code);
            }
        } break;
        case EVENT_CODE_KEY_RELEASED: {
            u16 key_code = context.data.d_u16[0];
            if (key_code == Input::Keys::B) {
                DF_DEBUG("Explicit - B key released!");
            }
            else {
                DF_DEBUG("'%c' key released in a window.", key_code);
            }
        } break;
    }
    return FALSE;
}
