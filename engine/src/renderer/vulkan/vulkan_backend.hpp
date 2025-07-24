#pragma once

#include <defines.hpp>
#include <platform/platform.hpp>
#include <renderer/renderer_types.inl>
#include <renderer/renderer_backend.hpp>

namespace Renderer {
class VulkanBackend : public Backend {
public:
    VulkanBackend(const char* application_name, platform_state* plat_state);
    ~VulkanBackend() = default;
    void Shutdown();
    b8 Initialize(const char* application_name, platform_state* plat_state);
    void OnResized(u16 width, u16 height);
    b8 BeginFrame(f32 delta_time);
    b8 EndFrame(f32 delta_time);
};
} // namespace Renderer
