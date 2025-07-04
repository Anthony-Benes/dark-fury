local vulkan_sdk = os.getenv("VULKAN_SDK")
if not vulkan_sdk then
    error("VULKAN_SDK environment variable is not set!")
end
workspace "DarkFury"
    configurations { "Debug", "Release" }
    location ".."
    language "C++"
    cppdialect "C++20"
	flags { "MultiProcessorCompile" }
	filter "action:gmake"
		buildoptions { "-g", "-Wall", "-Wextra", "-std=c++20" }
        buildcommands { "mkdir -p build && cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES .." }
include "projectSetup.lua"

NewProject {
    name = "engine",
    kind = "SharedLib",
    sourcesPath = "engine",
    defines = { "DF_EXPORT" },
    links = {
        "vulkan",
        "xcb",
        "X11",
        "X11-xcb",
        "xkbcommon",
        "vulkan-1" -- for Windows; harmless on Linux if missing
    }
}

-- TESTAPP PROJECT
NewProject {
    name = "testapp",
    kind = "ConsoleApp",
    sourcesPath = "testapp",
    defines = { "DF_IMPORT" },
    links = { "engine" }
}

-- Additional include path for testapp to find engine headers
project "testapp"
    includedirs {
        "../engine/src"
    }
