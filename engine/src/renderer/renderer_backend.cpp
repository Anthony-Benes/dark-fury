#include "renderer_backend.hpp"
#include <platform/platform.hpp>

namespace Renderer {
Backend::Backend([[maybe_unused]]const char* application_name, [[maybe_unused]]platform_state* plat_state) {
    // Expect the specific instances to call Initialize so this uses the right Initialize function
}

b8 Backend::BeginFrame([[maybe_unused]]f32 delta_time) {
    return true;
}

b8 Backend::EndFrame([[maybe_unused]]f32 delta_time) {
    return true;
}

} // namespace Renderer
