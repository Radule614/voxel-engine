project "VoxelEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/lib")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	debugdir "%{cfg.targetdir}"

	postbuildcommands {
            -- Windows:
            '{COPY} "%{prj.location}/assets" "%{cfg.targetdir}/../assets"'
        }

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
		"%{IncludeDir.PerlinNoise}",
		"%{IncludeDir.Entt}",
		"%{IncludeDir.AssetUtils}"
	}

	links
	{
		"OpenGLCore",
		"JoltPhysics"
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
