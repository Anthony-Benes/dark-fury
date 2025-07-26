#include "clock.hpp"
#include <platform/platform.hpp>

Clock::Clock() : start_time(platform_get_absolute_time()) {}

void Clock::Update() {
    if ( start_time != 0 ) { elapsed = platform_get_absolute_time() - start_time; }
}

void Clock::Start() {
    start_time = platform_get_absolute_time();
    elapsed    = 0;
}

void Clock::Stop() { start_time = 0; }
