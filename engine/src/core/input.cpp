#include "defines.hpp"
#include <core/input.hpp>

#include <core/event.hpp>
#include <core/df_memory.hpp>
#include <core/logger.hpp>

namespace Input {
struct keyboard_state {
    b8 keys[256];
};

struct mouse_state {
    i16 x;
    i16 y;
    u8 buttons[3]; // Buttons ( Left, Right, Middle )
};

struct input_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
};

static b8 initialized = FALSE;
static input_state state = {};

void Initialize() {
    df_zero_memory(&state, sizeof(input_state));
    initialized = TRUE;
    DF_INFO("Input subsystem initialized");
}

void Shutdown() {
    initialized = FALSE;
}

void Update(f64 delta_time) {
    if (!initialized) { return; }
    df_copy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    df_copy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

void ProcessKey(Keys key, b8 pressed) {
    if (state.keyboard_current.keys[key] != pressed) {
        state.keyboard_current.keys[key] = pressed;
        event_context context;
        context.data.d_u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

void ProcessButton(Buttons button, b8 pressed) {
    if (state.mouse_current.buttons[button] != pressed) {
        state.mouse_current.buttons[button] = pressed;
        event_context context;
        context.data.d_u16[0] = button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void ProcessMouseMove(i16 x, i16 y) {
    if (state.mouse_current.x != x || state.mouse_current.y != y) {
        if constexpr (DF_DEBUG_INPUT) {
            DF_DEBUG("Mouse pos: %i, %i!", x, y);
        }
        state.mouse_current.x = x;
        state.mouse_current.y = y;
        event_context context;
        context.data.d_u16[0] = x;
        context.data.d_u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

void ProcessMouseWheel(i8 z_delta) {
    event_context context;
    context.data.d_u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

b8 IsKeyDown(Keys key) {
    if (!initialized) { return FALSE; }
    return state.keyboard_current.keys[key] == TRUE;
}

b8 IsKeyUp(Keys key) {
    if (!initialized) { return TRUE; }
    return state.keyboard_current.keys[key] == FALSE;
}

b8 WasKeyDown(Keys key) {
    if (!initialized) { return FALSE; }
    return state.keyboard_previous.keys[key] == TRUE;
}

b8 WasKeyUp(Keys key) {
    if (!initialized) { return TRUE; }
    return state.keyboard_previous.keys[key] == FALSE;
}

b8 IsButtonDown(Buttons button) {
    if (!initialized) { return FALSE; }
    return state.mouse_current.buttons[button] == TRUE;
}

b8 IsButtonUp(Buttons button) {
    if (!initialized) { return TRUE; }
    return state.mouse_current.buttons[button] == FALSE;
}

b8 WasButtonDown(Buttons button) {
    if (!initialized) { return FALSE; }
    return state.mouse_previous.buttons[button] == TRUE;
}

b8 WasButtonUp(Buttons button) {
    if (!initialized) { return TRUE; }
    return state.mouse_previous.buttons[button] == FALSE;
}

void GetMousePosition(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mouse_current.x;
    *y = state.mouse_current.y;
}

void GetPreviousMousePosition(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;
}

} // namespace Input
