#pragma once

#include <defines.hpp>

namespace Engine::Event {
struct Context {
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

namespace Code {
enum class System : u16 {
    ApplicationQuit = 0x01,
    KeyPressed      = 0x02,
    KeyReleased     = 0x03,
    ButtonPressed   = 0x04,
    ButtonReleased  = 0x05,
    MouseMoved      = 0x06,
    MouseWheel      = 0x07,
    Resized         = 0x08,
    Invalid         = 0xff,
};
}  // namespace Code

typedef b8 (*F_on_event)(Code::System code, void* sender, void* listener_inst, Context data);

b8 Initialize();
void Shutdown();

DF_API b8 Register(Code::System code, void* listener, F_on_event on_event);

DF_API b8 Unregister(Code::System code, void* listener, F_on_event on_event);

DF_API b8 Fire(Code::System code, void* sender, Context context);
}  // namespace Engine::Event
