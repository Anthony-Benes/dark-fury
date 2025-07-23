local vulkan_sdk = os.getenv("VULKAN_SDK")
if not vulkan_sdk then
    error("VULKAN_SDK environment variable is not set!")
end
workspace "DarkFury"
configurations { "Debug", "Release" }
platforms { "x64" }
architecture = "x86_64"
location ".."
language "C++"
cppdialect "C++20"
flags { "MultiProcessorCompile" }
filter "action:gmake"
buildoptions { "-g", "-Wall", "-Wextra", "-std=c++20" }
buildcommands { "mkdir -p out && cd out && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES .." }
filter {}

local function projectPath(path)
    return '../' .. path
end
function NewProject(options)
    local o_name = options.name
    local o_kind = options.kind or "ConsoleApp"
    local sourcesPath = options.sourcesPath or o_name
    local outputName = options.outputName or o_name
    local extraDefines = options.defines or {}
    local extraLinks = options.links or {}
    local extraIncludeDirs = options.includedirs or {}
    project(o_name)
    location(projectPath('out') .. "/" .. o_name)
    kind(o_kind)
    targetname(o_kind == "SharedLib" and ("lib" .. outputName) or outputName)
    targetdir(projectPath("out") .. "/" .. o_name .. "/%{cfg.buildcfg}")
    objdir(projectPath("out/obj") .. "/" .. o_name .. "/%{cfg.buildcfg}")
    files {
        projectPath(sourcesPath) .. "/**.h",
        projectPath(sourcesPath) .. "/**.hpp",
        projectPath(sourcesPath) .. "/**.c",
        projectPath(sourcesPath) .. "/**.cpp"
    }
    includedirs {
        projectPath(sourcesPath) .. "/src",
        table.unpack(extraIncludeDirs)
    }
    libdirs {
        projectPath("out") .. "/engine/%{cfg.buildcfg}"
    }
    links(extraLinks)
    defines("DF_VULKAN")
    filter "system:linux"
    pic "On"
    if o_kind == "ConsoleApp" then
        linkoptions {
            "-Wl,-rpath,."
        }
    end
    filter "system:windows"
    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }
    if options.buildoutputs then
        buildoutputs(projectPath('out') .. options.buildoutputs)
    end
    if o_name == "engine" then
        includedirs {
            vulkan_sdk .. "/Include"
        }
        libdirs {
            vulkan_sdk .. "/Lib"
        }
    else
        postbuildcommands {
            'xcopy /Q /Y /I "..\\engine\\%{cfg.buildcfg}\\libengine.dll" "$(TargetDir)"'
        }
    end
    filter "configurations:Debug"
    symbols "On"
    defines {
        "_DEBUG"
    }
    defines(extraDefines)
    filter "configurations:Release"
    optimize "On"
    defines {
        "NDEBUG"
    }
    defines(extraDefines)
    filter {}
end
startproject "testapp"

local externals = dofile("../engine/external/external.lua")
local engineLinks = {}
if os.target() == "windows" then
    engineLinks = { "vulkan-1" }
elseif os.target() == "linux" then
    engineLinks = {
        "vulkan",
        "xcb",
        "X11",
        "X11-xcb",
        "xkbcommon"
    }
end
NewProject {
    name = "engine",
    kind = "SharedLib",
    sourcesPath = "engine",
    defines = { "DF_EXPORT" },
    links = table.join(engineLinks, externals.links),
    buildoutputs = "/engine/%{cfg.buildcfg}/libengine.dll",
    includedirs = externals.includes
}

NewProject {
    name = "testapp",
    kind = "ConsoleApp",
    sourcesPath = "testapp",
    defines = { "DF_IMPORT" },
    links = { "engine" },
    includedirs = { "../engine/src" }
}

include "generate_json.lua"
