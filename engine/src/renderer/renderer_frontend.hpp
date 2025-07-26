#pragma once

#include <defines.hpp>
#include <platform/platform.hpp>
#include <renderer/renderer_types.inl>

struct static_mesh_data;
struct platform_state;

namespace Renderer {
class Frontend {
  public:
    Frontend(const char* application_name, platform_state* plat_state);

    ~Frontend() { Shutdown(); }

    b8 Initialize(const char* application_name, platform_state* plat_state);
    void Shutdown();
    void OnResized(u16 width, u16 height);
    b8 DrawFrame(render_packet* packet);

  private:
};
}  // namespace Renderer
