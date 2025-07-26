#pragma once

#include <defines.hpp>

namespace Engine::Input {
#define DF_DEBUG_INPUT 0

enum Buttons {
    M_Invalid,
    M_Left,
    M_Right,
    M_Middle,
};

enum Keys : u16 {
    KeyInvalid  = 0x00,
    Backspace   = 0x08,
    Enter       = 0x0D,
    Tab         = 0x09,
    Shift       = 0x10,
    Control     = 0x11,
    Pause       = 0x13,
    Capital     = 0x14,
    Escape      = 0x1B,
    Convert     = 0x1C,
    NonConvert  = 0x1D,
    Accept      = 0x1E,
    ModeChange  = 0x1F,
    Space       = 0x20,
    PageUp      = 0x21,
    PageDown    = 0x22,
    End         = 0x23,
    Home        = 0x24,
    Left        = 0x25,
    Up          = 0x26,
    Right       = 0x27,
    Down        = 0x28,
    Select      = 0x29,
    Print       = 0x2A,
    Execute     = 0x2B,
    PrintScreen = 0x2C,
    Insert      = 0x2D,
    Delete      = 0x2E,
    Help        = 0x2F,
    K_0         = 0x30,
    K_1         = 0x31,
    K_2         = 0x32,
    K_3         = 0x33,
    K_4         = 0x34,
    K_5         = 0x35,
    K_6         = 0x36,
    K_7         = 0x37,
    K_8         = 0x38,
    K_9         = 0x39,
    A           = 0x41,
    B           = 0x42,
    C           = 0x43,
    D           = 0x44,
    E           = 0x45,
    F           = 0x46,
    G           = 0x47,
    H           = 0x48,
    I           = 0x49,
    J           = 0x4A,
    K           = 0x4B,
    L           = 0x4C,
    M           = 0x4D,
    N           = 0x4E,
    O           = 0x4F,
    P           = 0x50,
    Q           = 0x51,
    R           = 0x52,
    S           = 0x53,
    T           = 0x54,
    U           = 0x55,
    V           = 0x56,
    W           = 0x57,
    X           = 0x58,
    Y           = 0x59,
    Z           = 0x5A,
    LWin        = 0x5B,
    RWin        = 0x5C,
    Apps        = 0x5D,
    Sleep       = 0x5F,
    NumPad0     = 0x60,
    NumPad1     = 0x61,
    NumPad2     = 0x62,
    NumPad3     = 0x63,
    NumPad4     = 0x64,
    NumPad5     = 0x65,
    NumPad6     = 0x66,
    NumPad7     = 0x67,
    NumPad8     = 0x68,
    NumPad9     = 0x69,
    Multiply    = 0x6A,
    Add         = 0x6B,
    Separator   = 0x6C,
    Subtract    = 0x6D,
    Decimal     = 0x6E,
    Divide      = 0x6F,
    F1          = 0x70,
    F2          = 0x71,
    F3          = 0x72,
    F4          = 0x73,
    F5          = 0x74,
    F6          = 0x75,
    F7          = 0x76,
    F8          = 0x77,
    F9          = 0x78,
    F10         = 0x79,
    F11         = 0x7A,
    F12         = 0x7B,
    F13         = 0x7C,
    F14         = 0x7D,
    F15         = 0x7E,
    F16         = 0x7F,
    F17         = 0x80,
    F18         = 0x81,
    F19         = 0x82,
    F20         = 0x83,
    F21         = 0x84,
    F22         = 0x85,
    F23         = 0x86,
    F24         = 0x87,
    NumLock     = 0x90,
    Scroll      = 0x91,
    NumPadEqual = 0x92,
    LShift      = 0xA0,
    RShift      = 0xA1,
    LControl    = 0xA2,
    RControl    = 0xA3,
    LMenu       = 0xA4,
    RMenu       = 0xA5,
    Semicolon   = 0x3A,
    Apostrophe  = 0xDE,
    Quote       = Apostrophe,
    Equal       = 0xBB,
    Comma       = 0xBC,
    Minus       = 0xBD,
    Period      = 0xBE,
    Slash       = 0xBF,
    Grave       = 0xC0,
    LBracket    = 0xDB,
    Pipe        = 0xDC,
    Backslash   = Pipe,
    RBracket    = 0xDD,
};

void Initialize();
void Shutdown();
void Update(f64 delta_time);

DF_API b8 IsKeyDown(Keys key);
DF_API b8 IsKeyUp(Keys key);
DF_API b8 WasKeyDown(Keys key);
DF_API b8 WasKeyUp(Keys key);

void ProcessKey(Keys key, b8 pressed);

DF_API b8 IsButtonDown(Buttons button);
DF_API b8 IsButtonUp(Buttons button);
DF_API b8 WasButtonDown(Buttons button);
DF_API b8 WasButtonUp(Buttons button);
DF_API void GetMousePosition(i32* x, i32* y);
DF_API void GetPreviousMousePosition(i32* x, i32* y);

void ProcessButton(Buttons button, b8 pressed);
void ProcessMouseMove(i16 x, i16 y);
void ProcessMouseWheel(i8 z_delta);

}  // namespace Engine::Input
