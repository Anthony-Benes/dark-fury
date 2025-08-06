#pragma once

#include <defines.hpp>
#include <core/application.hpp>
#include <core/df_memory.hpp>
#if defined(_DEBUG)
#include <core/logger.hpp>
#endif

using namespace Engine;

extern Application* CreateApplication(AppCommandLineArgs args);

int main(int argc, char** argv) {
#if defined(_DEBUG)
    Log::Info("Running with Debug");
#endif
    Memory::Initialize();
    auto game = CreateApplication({ argc, argv });
    game->Run();
    delete game;
    Memory::Shutdown();
    return 0;
}
