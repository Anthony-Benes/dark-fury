#include "engpch.h"
#include "Engine/Renderer/RenderCommand.hpp"

namespace Engine {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}