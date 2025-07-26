#include "event.hpp"
#include "core/logger.hpp"

#include <containers/darray.hpp>
#include <core/df_memory.hpp>
#include <vector>

namespace Engine::Event {
typedef struct registered_event {
    void* listener;
    F_on_event callback;
} registered_event;

typedef struct event_code_entry {
    DArray<registered_event> events;
} event_code_entry;

#define MAX_MESSAGE_CODES 16384

typedef struct event_system_state {
    event_code_entry registered[MAX_MESSAGE_CODES];
} event_system_state;

static b8 is_initialized = false;
static event_system_state state;

b8 Initialize() {
    if ( is_initialized == true ) { return false; }
    Memory::df_zero_memory(&state, sizeof(state));
    is_initialized = true;
    return true;
}

void Shutdown() {
    for ( u16 i = 0; i < MAX_MESSAGE_CODES; i++ ) { state.registered[i].events.clear(); }
}

b8 Register(Code::System code, void* listener, F_on_event on_event) {
    if ( is_initialized == false ) { return false; }
    auto codeInt                     = static_cast<u16>(code);
    DArray<registered_event>& events = state.registered[codeInt].events;
    for ( u64 i = 0; i < events.size(); i++ ) {
        if ( events[i].listener == listener ) {
            Log::Warn("Event has already been registered. Did not create another.");
            return false;
        }
    }
    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    events.push(event);
    return true;
}

b8 Unregister(Code::System code, void* listener, F_on_event on_event) {
    if ( !is_initialized ) { return false; }
    auto codeInt                     = static_cast<u16>(code);
    DArray<registered_event>& events = state.registered[codeInt].events;
    for ( u64 i = 0; i < events.size(); i++ ) {
        registered_event e = events[i];
        if ( e.listener == listener && e.callback == on_event ) {
            events.pop_at(i);
            return true;
        }
    }
    Log::Warn("Event not found. Failed to unregister.");
    return false;
}

b8 Fire(Code::System code, void* sender, Event::Context context) {
    if ( !is_initialized ) { return false; }
    auto codeInt                     = static_cast<u16>(code);
    DArray<registered_event>& events = state.registered[codeInt].events;
    for ( u64 i = 0; i < events.size(); i++ ) {
        registered_event e = events[i];
        if ( e.callback(code, sender, e.listener, context) ) {
            // Message has been handled and consumed. Don't continue sending to other listeners
            return true;
        }
    }
    return false;
}

}  // namespace Engine::Event
