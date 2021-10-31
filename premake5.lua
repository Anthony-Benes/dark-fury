workspace "Dark-Fury"
	architecture "x64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Dark-Fury/ext_lib/GLFW/include"
IncludeDir["Glad"] = "Dark-Fury/ext_lib/Glad/include"
IncludeDir["ImGui"] = "Dark-Fury/ext_lib/imgui"
IncludeDir["glm"] = "Dark-Fury/ext_lib/glm"
IncludeDir["stb_image"] = "Dark-Fury/ext_lib/stb_image"

group "Dependencies"
include "Dark-Fury/ext_lib/GLFW"
include "Dark-Fury/ext_lib/Glad"
include "Dark-Fury/ext_lib/imgui"
group ""

project "Dark-Fury"
	location "Dark-Fury"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "engpch.h"
	pchsource("%{prj.name}/src/engpch.cpp")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/ext_lib/glm/glm/**.hpp",
		"%{prj.name}/ext_lib/glm/glm/**.inl",
		"%{prj.name}/ext_lib/stb_image/**.h",
		"%{prj.name}/ext_lib/stb_image/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "ENG_RELEASE"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Dist"
		defines "ENG_DIST"
		runtime "Release"
		optimize "on"

project "SampleGame"
	location "SampleGame"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Dark-Fury/src",
		"Dark-Fury/ext_lib",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Dark-Fury"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "ENG_RELEASE"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Dist"
		defines "ENG_DIST"
		runtime "Release"
		optimize "on"

project "StarterGame"
	location "StarterGame"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Dark-Fury/src",
		"Dark-Fury/ext_lib",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Dark-Fury"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "ENG_RELEASE"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Dist"
		defines "ENG_DIST"
		runtime "Release"
		optimize "on"

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Dark-Fury/src",
		"Dark-Fury/ext_lib",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Dark-Fury"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "ENG_RELEASE"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Dist"
		defines "ENG_DIST"
		runtime "Release"
		optimize "on"