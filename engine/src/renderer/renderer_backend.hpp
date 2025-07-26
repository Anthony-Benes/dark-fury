#pragma once

#include <defines.hpp>
#include <platform/platform.hpp>
#include <renderer/renderer_types.inl>

namespace Renderer {
class Backend {
  public:
    Backend(const char* application_name, platform_state* plat_state);
    ~Backend()                                                                      = default;
    virtual void Shutdown()                                                         = 0;
    virtual b8 Initialize(const char* application_name, platform_state* plat_state) = 0;
    virtual void OnResized(u16 width, u16 height)                                   = 0;
    b8 BeginFrame(f32 delta_time);
    b8 EndFrame(f32 delta_time);

  private:
    u64 frame_number = 0;
};

Backend* CreateBackend(const char* application_name, platform_state* plat_state);
}  // namespace Renderer
