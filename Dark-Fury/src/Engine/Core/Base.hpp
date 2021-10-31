#pragma once

#include "engpch.h"

#include "Engine/Core/PlatformDetection.h"

// DLL support
#ifdef ENG_PLATFORM_WINDOWS
#if ENG_DYNAMIC_LINK
#ifdef ENG_BUILD_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API
#endif
#else
#error Engine only supports Windows!
#endif // End of DLL support

#ifdef ENG_DEBUG
#if defined(ENG_PLATFORM_WINDOWS)
#define ENG_DEBUGBREAK() __debugbreak()
#elif defined(ENG_PLATFORM_LINUX)
#include <signal.h>
#define ENG_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define ENG_ENABLE_ASSERTS
#else
#define ENG_DEBUGBREAK()
#endif

#define ENG_EXPAND_MACRO(x) x
#define ENG_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define ENG_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define DEG_TO_RAD(degrees)	((degrees) * 0.017453f)

namespace Engine {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "Engine/Core/Log.hpp"
#include "Engine/Core/Assert.hpp"