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

static b8 initialized = false;
static input_state state = {};

void Initialize() {
    Memory::df_zero_memory(&state, sizeof(input_state));
    initialized = true;
    Logger::Info("Input subsystem initialized");
}

void Shutdown() {
    initialized = false;
}

void Update(f64 delta_time) {
    if (!initialized) { return; }
    Memory::df_copy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    Memory::df_copy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

void ProcessKey(Keys key, b8 pressed) {
    if (state.keyboard_current.keys[key] != pressed) {
        state.keyboard_current.keys[key] = pressed;
        Event::Context context;
        context.data.d_u16[0] = key;
        Event::Fire(pressed ? Event::Code::System::KeyPressed : Event::Code::System::KeyReleased, 0, context);
    }
}

void ProcessButton(Buttons button, b8 pressed) {
    if (state.mouse_current.buttons[button] != static_cast<u8>(pressed)) {
        state.mouse_current.buttons[button] = pressed;
        Event::Context context;
        context.data.d_u16[0] = button;
        Event::Fire(pressed ? Event::Code::System::ButtonPressed : Event::Code::System::ButtonReleased, 0, context);
    }
}

void ProcessMouseMove(i16 x, i16 y) {
    if (state.mouse_current.x != x || state.mouse_current.y != y) {
        if constexpr (DF_DEBUG_INPUT) {
            Logger::Debug("Mouse pos: %i, %i!", x, y);
        }
        state.mouse_current.x = x;
        state.mouse_current.y = y;
        Event::Context context;
        context.data.d_u16[0] = x;
        context.data.d_u16[1] = y;
        Event::Fire(Event::Code::System::MouseMoved, 0, context);
    }
}

void ProcessMouseWheel(i8 z_delta) {
    Event::Context context;
    context.data.d_u8[0] = z_delta;
    Event::Fire(Event::Code::System::MouseWheel, 0, context);
}

b8 IsKeyDown(Keys key) {
    if (!initialized) { return false; }
    return state.keyboard_current.keys[key] == true;
}

b8 IsKeyUp(Keys key) {
    if (!initialized) { return true; }
    return state.keyboard_current.keys[key] == false;
}

b8 WasKeyDown(Keys key) {
    if (!initialized) { return false; }
    return state.keyboard_previous.keys[key] == true;
}

b8 WasKeyUp(Keys key) {
    if (!initialized) { return true; }
    return state.keyboard_previous.keys[key] == false;
}

b8 IsButtonDown(Buttons button) {
    if (!initialized) { return false; }
    return state.mouse_current.buttons[button] == 1;
}

b8 IsButtonUp(Buttons button) {
    if (!initialized) { return true; }
    return state.mouse_current.buttons[button] == 0;
}

b8 WasButtonDown(Buttons button) {
    if (!initialized) { return false; }
    return state.mouse_previous.buttons[button] == 1;
}

b8 WasButtonUp(Buttons button) {
    if (!initialized) { return true; }
    return state.mouse_previous.buttons[button] == 0;
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
