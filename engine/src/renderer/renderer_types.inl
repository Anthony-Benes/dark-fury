#pragma once

#include <defines.hpp>

// Renderer Selection
#if defined(DF_VULKAN)
#define RENDER_TYPE_VULKAN
#else
#error No Valid Renderer Set Up!
#endif

extern struct platform_state* plat_state;

namespace Renderer {
typedef struct render_packet {
    f32 delta_time;
} render_packet;
}  // namespace Renderer
