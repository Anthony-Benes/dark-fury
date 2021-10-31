#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Core/Application.hpp"

#ifdef ENG_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) {
	Engine::Log::Init();

	ENG_PROFILE_BEGIN_SESSION("Startup", "EngineProfile-Startup.json");
	auto app = Engine::CreateApplication({ argc, argv });
	ENG_PROFILE_END_SESSION();

	ENG_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Runtime.json");
	app->Run();
	ENG_PROFILE_END_SESSION();

	ENG_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Shutdown.json");
	delete app;
	ENG_PROFILE_END_SESSION();
}

#endif