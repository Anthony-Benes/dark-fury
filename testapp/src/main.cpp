#include <platform/platform.hpp>
#include <core/logger.hpp>
#include <core/asserts.hpp>

int main(void) {
    DF_FATAL("Test Fatal!");
    DF_WARN("Test format %f", 3.14f);
    DF_INFO("Hello World");

    platform_state state;
    if (platform_startup(&state, "Dark Fury Test App", 100, 100, 1280, 720)) {
        while (TRUE) {
            platform_pump_message(&state);
        }
    }
    platform_shutdown(&state);
	return 0;
}
