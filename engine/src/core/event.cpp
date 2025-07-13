#include "event.hpp"
#include "core/logger.hpp"

#include <core/df_memory.hpp>
#include <containers/darray.hpp>
#include <vector>

namespace Event {
    typedef struct registered_event {
        void* listener;
        F_on_event callback;
    } registered_event;

    typedef struct event_code_entry {
        registered_event* events;
    } event_code_entry;

#define MAX_MESSAGE_CODES 16384

    typedef struct event_system_state {
        event_code_entry registered[MAX_MESSAGE_CODES];
    } event_system_state;

    static b8 is_initialized = false;
    static event_system_state state;

    b8 Initialize() {
        if (is_initialized == true) {
            return false;
        }
        Memory::df_zero_memory(&state, sizeof(state));
        is_initialized = true;
        return true;
    }

    void Shutdown() {
        for (u16 i = 0; i < MAX_MESSAGE_CODES; i++) {
            if (state.registered[i].events != 0) {
                darray_destroy(state.registered[i].events);
                state.registered[i].events = 0;
            }
        }
    }

    b8 Register(Code::System code, void* listener, F_on_event on_event) {
        if (is_initialized == false) {
            return false;
        }
        auto codeInt = static_cast<u16>(code);
        if (state.registered[codeInt].events == 0) {
            state.registered[codeInt].events = (registered_event*)darray_create(registered_event);
        }
        u64 registered_count = darray_length(state.registered[codeInt].events);
        for (u64 i = 0; i < registered_count; i++) {
            if (state.registered[codeInt].events[i].listener == listener) {
                Logger::Warn("Event has already been registered. Did not create another.");
                return false;
            }
        }
        registered_event event;
        event.listener = listener;
        event.callback = on_event;
        darray_push(state.registered[codeInt].events, &event);
        return true;
    }

    b8 Unregister(Code::System code, void* listener, F_on_event on_event) {
        if (is_initialized == false) {
            return false;
        }
        auto codeInt = static_cast<u16>(code);
        if (state.registered[codeInt].events == 0) {
            Logger::Warn("Event does not exist. Cannot unregister.");
            return false;
        }
        u64 registered_count = darray_length(state.registered[codeInt].events);
        for (u64 i = 0; i < registered_count; i++) {
            registered_event e = state.registered[codeInt].events[i];
            if (e.listener == listener && e.callback == on_event) {
                registered_event popped_event;
                darray_pop_at(state.registered[codeInt].events, i, &popped_event);
                return true;
            }
        }
        Logger::Warn("Event not found. Failed to unregister.");
        return false;
    }

    b8 Fire(Code::System code, void* sender, Event::Context context) {
        if (is_initialized == false) {
            return false;
        }
        auto codeInt = static_cast<u16>(code);
        if (state.registered[codeInt].events == 0) {
            return false;
        }
        u64 registered_count = darray_length(state.registered[codeInt].events);
        for (u64 i = 0; i < registered_count; i++) {
            registered_event e = state.registered[codeInt].events[i];
            if (e.callback(code, sender, e.listener, context)) {
                // Message has been handled and consumed. Don't continue sending to other listeners
                return true;
            }
        }
        return false;
    }

} // namespace Event