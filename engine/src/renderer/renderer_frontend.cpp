#include "renderer_frontend.hpp"
#include "renderer/renderer_types.inl"
#include <core/df_memory.hpp>
#include <core/logger.hpp>
#include <platform/platform.hpp>
#include <renderer/renderer_backend.hpp>

#if defined(RENDER_TYPE_VULKAN)
#include <renderer/vulkan/vulkan_backend.hpp>
#endif

namespace Renderer {
static Backend* backend = nullptr;
Frontend::Frontend(const char* application_name, platform_state* plat_state) {
    Initialize(application_name, plat_state);
}

b8 Frontend::Initialize(const char* application_name, platform_state* plat_state) {
    backend = CreateBackend(application_name, plat_state);
    return true;
}

void Frontend::Shutdown() {
    backend->Shutdown();
}

b8 Frontend::DrawFrame(render_packet* packet) {
    if (backend->BeginFrame(packet->delta_time)) {
        if (!backend->EndFrame(packet->delta_time)) {
            Engine::Log::Error("Backend::EndFrame failed. Application shutting down...");
            return false;
        }
    }
    return true;
}

} // namespace Renderer
