#pragma once

#include <defines.hpp>

struct event_context {
    union {
        i64 d_i64[2];
        u64 d_u64[2];
        f64 d_f64[2];
        i32 d_i32[4];
        u32 d_u32[4];
        f32 d_f32[4];
        i16 d_i16[8];
        u16 d_u16[8];
        i8 d_i8[16];
        u8 d_u8[16];
        char d_c[16];
    } data;
};

typedef b8 (*F_on_event)(u16 code, void* sender, void* listener_inst, event_context data);

b8 event_initialize();
void event_shutdown();

DF_API b8 event_register(u16 code, void* listener, F_on_event on_event);

DF_API b8 event_unregister(u16 code, void* listener, F_on_event on_event);

DF_API b8 event_fire(u16 code, void* sender, event_context context);

enum SYSTEM_event_code {
    EVENT_CODE_APPLICATION_QUIT = 0x01,
    EVENT_CODE_KEY_PRESSED = 0x02,
    EVENT_CODE_KEY_RELEASED = 0x03,
    EVENT_CODE_BUTTON_PRESSED = 0x04,
    EVENT_CODE_BUTTON_RELEASED = 0x05,
    EVENT_CODE_MOUSE_MOVED = 0x06,
    EVENT_CODE_MOUSE_WHEEL = 0x07,
    EVENT_CODE_RESIZED = 0x08,
    MAX_EVENT_CODE = 0xff
};
