#include "engpch.h"
#include "Engine/Core/Window.hpp"

#ifdef ENG_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.hpp"
#endif

namespace Engine
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef ENG_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		ENG_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}