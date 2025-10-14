project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir)
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
		"%{IncludeDir.Entt}",
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
