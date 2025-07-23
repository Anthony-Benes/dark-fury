#pragma once

#include <defines.hpp>

class Clock {
public:
    Clock();
    ~Clock() = default;
    void Update();
    void Start();
    void Stop();
    f64 TElapse() { return elapsed; }
    f64 TStart() { return start_time; }
private:
    f64 start_time;
    f64 elapsed = 0;
};
