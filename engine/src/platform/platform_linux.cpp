#include "platform.hpp"

#if DF_PLATFORM_LINUX
#include <containers/darray.hpp>
#include <core/event.hpp>
#include <core/input.hpp>
#include <core/logger.hpp>

#include <X11/X.h>
#include <xcb/xproto.h>
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct internal_state {
    Display *display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
} internal_state;

Engine::Input::Keys translate_keycode(u32 x_keycode);

b8 platform_startup(platform_state* plat_state, const char* application_name, i32 x, i32 y, i32 width, i32 height) {
    plat_state->internal_state = platform_allocate(sizeof(internal_state));
    internal_state* state = (internal_state*)plat_state->internal_state;
    state->display = XOpenDisplay(NULL);
    XAutoRepeatOff(state->display);
    state->connection = XGetXCBConnection(state->display);
    if (xcb_connection_has_error(state->connection)) {
        Engine::Log::Fatal("Failed to connect to X server via XCB.");
        return false;
    }
    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    int screen_p = 0;
    for (i32 s = screen_p; s > 0; s--) {
        xcb_screen_next(&it);
    }
    state->screen = it.data;
    state->window = xcb_generate_id(state->connection);
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    u32 value_list[] = { state->screen->black_pixel, event_values };
    xcb_create_window(state->connection, XCB_COPY_FROM_PARENT,
                      state->window, state->screen->root,
                      x, y, width, height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      state->screen->root_visual, event_mask,
                      value_list);
    xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                        strlen(application_name), application_name);
    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(state->connection, 0,
                                                                strlen("WM_DELETE_WINDOW"),
                                                                "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(state->connection, 0,
                                                                strlen("WM_PROTOCOLS"),
                                                                "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(state->connection,
                                                                     wm_delete_cookie, NULL);
    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(state->connection,
                                                                     wm_protocols_cookie, NULL);
    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;
    xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window,
                        wm_protocols_reply->atom, 4, 32, 1, &wm_delete_reply->atom);
    xcb_map_window(state->connection, state->window);
    i32 stream_result = xcb_flush(state->connection);
    if (stream_result <= 0) {
        Engine::Log::Fatal("An error occurred when flushing the stream: %d", stream_result);
        return false;
    }
    return true;
}

void platform_shutdown(platform_state* plat_state) {
    internal_state* state = (internal_state*)plat_state->internal_state;
    XAutoRepeatOn(state->display);
    xcb_destroy_window(state->connection, state->window);
}

b8 platform_pump_message(platform_state* plat_state) {
    internal_state* state = (internal_state*)plat_state->internal_state;
    xcb_client_message_event_t* cm;
    b8 quit_flagged = false;
    while (xcb_generic_event_t* event = xcb_poll_for_event(state->connection)) {
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                xcb_key_press_event_t* kb_event = (xcb_key_press_event_t*)event;
                b8 pressed = event->response_type == XCB_KEY_PRESS;
                xcb_keycode_t code = kb_event->detail;
                KeySym key_sym = XkbKeycodeToKeysym(state->display, (KeyCode)code, 0, code & ShiftMask ? 1 : 0);
                Engine::Input::Keys key = translate_keycode(key_sym);
                Engine::Input::ProcessKey(key, pressed);
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                xcb_button_press_event_t* mouse_event = (xcb_button_press_event_t*)event;
                b8 pressed = event->response_type == XCB_BUTTON_PRESS;
                Engine::Input::Buttons button = Engine::Input::Buttons::M_Invalid;
                switch (mouse_event->detail) {
                    case XCB_BUTTON_INDEX_1:
                        button = Engine::Input::Buttons::M_Left;
                        break;
                    case XCB_BUTTON_INDEX_2:
                        button = Engine::Input::Buttons::M_Middle;
                        break;
                    case XCB_BUTTON_INDEX_3:
                        button = Engine::Input::Buttons::M_Right;
                        break;
                }
                Engine::Input::ProcessButton(button, pressed);
            } break;
            case XCB_MOTION_NOTIFY: {
                xcb_motion_notify_event_t* move_event = (xcb_motion_notify_event_t*)event;
                Engine::Input::ProcessMouseMove(move_event->event_x, move_event->event_y);
            } break;
            case XCB_CONFIGURE_NOTIFY: {
            } break;
            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t*)event;
                if (cm->data.data32[0] == state->wm_delete_win) {
                    quit_flagged = true;
                }
            } break;
            default:
                break;
        }
        platform_free(event);
    }
    return !quit_flagged;
}

void* platform_allocate(u64 size, [[maybe_unused]]b8 aligned) { return malloc(size); }
void platform_free(void* block, [[maybe_unused]]b8 aligned) { free(block); }
void* platform_zero_memory(void* block, u64 size) { return memset(block, 0, size); }
void* platform_copy_memory(void* dest, const void* source, u64 size) { return memcpy(dest, source, size); }
void* platform_set_memory(void* dest, i32 value, u64 size) { return memset(dest, value, size); }

void platform_console_write(const char* message, u8 color) {
    const char* color_strings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
    printf("\033[%sm%s\033[0m", color_strings[color], message);
}
void platform_console_write_error(const char* message, u8 color) {
    const char* color_strings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
    printf("\033[%sm%s\033[0m", color_strings[color], message);
}

f64 platform_get_absolute_time() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void platform_sleep(u64 ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if (ms >= 1000) {
        sleep(ms / 1000);
    }
    usleep((ms % 1000) * 1000);
#endif
}

void platform_get_required_extension_names(Engine::DArray<const char *>*names_darray) {
    names_darray->push("VK_KHR_xcb_surface");
}

Engine::Input::Keys translate_keycode(u32 x_keycode) {
    switch (x_keycode) {
        case XK_BackSpace:
            return Engine::Input::Keys::Backspace;
        case XK_Return:
            return Engine::Input::Keys::Enter;
        case XK_Tab:
            return Engine::Input::Keys::Tab;
        case XK_Pause:
            return Engine::Input::Keys::Pause;
        case XK_Caps_Lock:
            return Engine::Input::Keys::Capital;
        case XK_Escape:
            return Engine::Input::Keys::Escape;
            // Not supported
            // case : return Engine::Input::Keys::Convert;
            // case : return Engine::Input::Keys::NonConvert;
            // case : return Engine::Input::Keys::Accept;
        case XK_Mode_switch:
            return Engine::Input::Keys::ModeChange;

        case XK_space:
            return Engine::Input::Keys::Space;
        case XK_Prior:
            return Engine::Input::Keys::PageUp;
        case XK_Next:
            return Engine::Input::Keys::PageDown;
        case XK_End:
            return Engine::Input::Keys::End;
        case XK_Home:
            return Engine::Input::Keys::Home;
        case XK_Left:
            return Engine::Input::Keys::Left;
        case XK_Up:
            return Engine::Input::Keys::Up;
        case XK_Right:
            return Engine::Input::Keys::Right;
        case XK_Down:
            return Engine::Input::Keys::Down;
        case XK_Select:
            return Engine::Input::Keys::Select;
        case XK_Print:
            return Engine::Input::Keys::Print;
        case XK_Execute:
            return Engine::Input::Keys::Execute;
        // case XK_snapshot: return Engine::Input::Keys::SNAPSHOT; // not supported
        case XK_Insert:
            return Engine::Input::Keys::Insert;
        case XK_Delete:
            return Engine::Input::Keys::Delete;
        case XK_Help:
            return Engine::Input::Keys::Help;
        case XK_Meta_L:
        case XK_Super_L:
            return Engine::Input::Keys::LWin;
        case XK_Meta_R:
        case XK_Super_R:
            return Engine::Input::Keys::RWin;
            // case XK_apps: return Engine::Input::Keys::Apps; // not supported
            // case XK_sleep: return Engine::Input::Keys::Sleep; //not supported
        case XK_KP_0:
            return Engine::Input::Keys::NumPad0;
        case XK_KP_1:
            return Engine::Input::Keys::NumPad1;
        case XK_KP_2:
            return Engine::Input::Keys::NumPad2;
        case XK_KP_3:
            return Engine::Input::Keys::NumPad3;
        case XK_KP_4:
            return Engine::Input::Keys::NumPad4;
        case XK_KP_5:
            return Engine::Input::Keys::NumPad5;
        case XK_KP_6:
            return Engine::Input::Keys::NumPad6;
        case XK_KP_7:
            return Engine::Input::Keys::NumPad7;
        case XK_KP_8:
            return Engine::Input::Keys::NumPad8;
        case XK_KP_9:
            return Engine::Input::Keys::NumPad9;
        case XK_multiply:
            return Engine::Input::Keys::Multiply;
        case XK_KP_Add:
            return Engine::Input::Keys::Add;
        case XK_KP_Separator:
            return Engine::Input::Keys::Separator;
        case XK_KP_Subtract:
            return Engine::Input::Keys::Subtract;
        case XK_KP_Decimal:
            return Engine::Input::Keys::Decimal;
        case XK_KP_Divide:
            return Engine::Input::Keys::Divide;
        case XK_F1:
            return Engine::Input::Keys::F1;
        case XK_F2:
            return Engine::Input::Keys::F2;
        case XK_F3:
            return Engine::Input::Keys::F3;
        case XK_F4:
            return Engine::Input::Keys::F4;
        case XK_F5:
            return Engine::Input::Keys::F5;
        case XK_F6:
            return Engine::Input::Keys::F6;
        case XK_F7:
            return Engine::Input::Keys::F7;
        case XK_F8:
            return Engine::Input::Keys::F8;
        case XK_F9:
            return Engine::Input::Keys::F9;
        case XK_F10:
            return Engine::Input::Keys::F10;
        case XK_F11:
            return Engine::Input::Keys::F11;
        case XK_F12:
            return Engine::Input::Keys::F12;
        case XK_F13:
            return Engine::Input::Keys::F13;
        case XK_F14:
            return Engine::Input::Keys::F14;
        case XK_F15:
            return Engine::Input::Keys::F15;
        case XK_F16:
            return Engine::Input::Keys::F16;
        case XK_F17:
            return Engine::Input::Keys::F17;
        case XK_F18:
            return Engine::Input::Keys::F18;
        case XK_F19:
            return Engine::Input::Keys::F19;
        case XK_F20:
            return Engine::Input::Keys::F20;
        case XK_F21:
            return Engine::Input::Keys::F21;
        case XK_F22:
            return Engine::Input::Keys::F22;
        case XK_F23:
            return Engine::Input::Keys::F23;
        case XK_F24:
            return Engine::Input::Keys::F24;
        case XK_Num_Lock:
            return Engine::Input::Keys::NumLock;
        case XK_Scroll_Lock:
            return Engine::Input::Keys::Scroll;
        case XK_KP_Equal:
            return Engine::Input::Keys::NumPadEqual;
        // case XK_Shift: return Engine::Input::Keys::Shift;
        // case XK_Control: return Engine::Input::Keys::Control;
        case XK_Shift_L:
            return Engine::Input::Keys::LShift;
        case XK_Shift_R:
            return Engine::Input::Keys::RShift;
        case XK_Control_L:
            return Engine::Input::Keys::LControl;
        case XK_Control_R:
            return Engine::Input::Keys::RControl;
        case XK_Alt_L:
            return Engine::Input::Keys::LMenu;
        case XK_Alt_R:
            return Engine::Input::Keys::RMenu;
        case XK_semicolon:
            return Engine::Input::Keys::Semicolon;
        case XK_plus:
            return Engine::Input::Keys::Equal;
        case XK_comma:
            return Engine::Input::Keys::Comma;
        case XK_minus:
            return Engine::Input::Keys::Minus;
        case XK_period:
            return Engine::Input::Keys::Period;
        case XK_slash:
            return Engine::Input::Keys::Slash;
        case XK_grave:
            return Engine::Input::Keys::Grave;
        case XK_0:
            return Engine::Input::Keys::K_0;
        case XK_1:
            return Engine::Input::Keys::K_1;
        case XK_2:
            return Engine::Input::Keys::K_2;
        case XK_3:
            return Engine::Input::Keys::K_3;
        case XK_4:
            return Engine::Input::Keys::K_4;
        case XK_5:
            return Engine::Input::Keys::K_5;
        case XK_6:
            return Engine::Input::Keys::K_6;
        case XK_7:
            return Engine::Input::Keys::K_7;
        case XK_8:
            return Engine::Input::Keys::K_8;
        case XK_9:
            return Engine::Input::Keys::K_9;
        case XK_a:
        case XK_A:
            return Engine::Input::Keys::A;
        case XK_b:
        case XK_B:
            return Engine::Input::Keys::B;
        case XK_c:
        case XK_C:
            return Engine::Input::Keys::C;
        case XK_d:
        case XK_D:
            return Engine::Input::Keys::D;
        case XK_e:
        case XK_E:
            return Engine::Input::Keys::E;
        case XK_f:
        case XK_F:
            return Engine::Input::Keys::F;
        case XK_g:
        case XK_G:
            return Engine::Input::Keys::G;
        case XK_h:
        case XK_H:
            return Engine::Input::Keys::H;
        case XK_i:
        case XK_I:
            return Engine::Input::Keys::I;
        case XK_j:
        case XK_J:
            return Engine::Input::Keys::J;
        case XK_k:
        case XK_K:
            return Engine::Input::Keys::K;
        case XK_l:
        case XK_L:
            return Engine::Input::Keys::L;
        case XK_m:
        case XK_M:
            return Engine::Input::Keys::M;
        case XK_n:
        case XK_N:
            return Engine::Input::Keys::N;
        case XK_o:
        case XK_O:
            return Engine::Input::Keys::O;
        case XK_p:
        case XK_P:
            return Engine::Input::Keys::P;
        case XK_q:
        case XK_Q:
            return Engine::Input::Keys::Q;
        case XK_r:
        case XK_R:
            return Engine::Input::Keys::R;
        case XK_s:
        case XK_S:
            return Engine::Input::Keys::S;
        case XK_t:
        case XK_T:
            return Engine::Input::Keys::T;
        case XK_u:
        case XK_U:
            return Engine::Input::Keys::U;
        case XK_v:
        case XK_V:
            return Engine::Input::Keys::V;
        case XK_w:
        case XK_W:
            return Engine::Input::Keys::W;
        case XK_x:
        case XK_X:
            return Engine::Input::Keys::X;
        case XK_y:
        case XK_Y:
            return Engine::Input::Keys::Y;
        case XK_z:
        case XK_Z:
            return Engine::Input::Keys::Z;
        default:
            return Engine::Input::Keys::KeyInvalid;
    }
}

#endif
