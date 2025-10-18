project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir)
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	debugdir "%{cfg.targetdir}"

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
		"../VoxelEngine/%{IncludeDir.JoltPhysics}",
		"../VoxelEngine/%{IncludeDir.Assimp}",
		"../VoxelEngine/%{IncludeDir.PerlinNoise}",
		"../VoxelEngine/%{IncludeDir.Entt}",
        "../VoxelEngine/src"
	}

	links
	{
		"OpenGLCore",
		"JoltPhysics",
		"Assimp",
		"VoxelEngine"
	}

    filter "system:windows"
        systemversion "latest"

        filter "configurations:Debug"
            runtime "Debug"    -- -> /MTd (static debug)
        filter "configurations:Release"
            runtime "Release"  -- -> /MT  (static release)