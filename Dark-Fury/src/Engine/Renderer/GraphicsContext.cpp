#include "engpch.h"
#include "Engine/Renderer/GraphicsContext.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Platform/OpenGL/OpenGLContext.hpp"

namespace Engine {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    ENG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}