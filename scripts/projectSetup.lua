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
    project(o_name)
		location(projectPath('build'))
        kind(o_kind)
        targetname(o_kind == "SharedLib" and ("lib" .. outputName) or outputName)
        targetdir(projectPath("bin") .. "/%{cfg.buildcfg}/" .. o_name)
        objdir(projectPath("build/obj") .. "/%{cfg.buildcfg}/" .. o_name)
        files {
            projectPath(sourcesPath) .. "/**.c",
            projectPath(sourcesPath) .. "/**.cpp",
            projectPath(sourcesPath) .. "/**.h",
            projectPath(sourcesPath) .. "/**.hpp"
        }
        includedirs {
            projectPath(sourcesPath) .. "/src"
        }
        libdirs {
            projectPath("bin") .. "/%{cfg.buildcfg}/engine"
        }
        links(extraLinks)
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
