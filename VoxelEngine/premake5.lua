project "VoxelEngine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.hpp",
		"src/**.cpp",
		"vendor/PerlinNoise/**.hpp",
		"vendor/Entt/**.hpp"
	}

	includedirs
	{
		"../OpenGLCore/vendor/spdlog/include",
		"../OpenGLCore/src",
		"../OpenGLCore/vendor",
		"../OpenGLCore/%{IncludeDir.glm}",
		"../OpenGLCore/%{IncludeDir.Glad}",
		"../OpenGLCore/%{IncludeDir.ImGui}",
		"%{IncludeDir.JoltPhysics}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.PerlinNoise}",
		"%{IncludeDir.Entt}"
	}

	links
	{
		"OpenGLCore",
		"JoltPhysics",
		"Assimp"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLCORE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "GLCORE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
        optimize "on"
