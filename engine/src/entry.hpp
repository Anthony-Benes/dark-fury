#pragma once

#include <defines.hpp>
#include <core/application.hpp>
#include <core/df_memory.hpp>

using namespace Engine;

extern Application* CreateApplication(AppCommandLineArgs args);

int main(int argc, char** argv) {
    Memory::Initialize();
    auto game = CreateApplication({argc, argv});
    game->Run();
    delete game;
    Memory::Shutdown();
    return 0;
}
