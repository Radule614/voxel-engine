workspace "VoxelEngine"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
-- Include directories relative to OpenGLCore
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"
-- Include directories relative to VoxelEngine
IncludeDir["JoltPhysics"] = "vendor/JoltPhysics"
IncludeDir["PerlinNoise"] = "vendor/PerlinNoise"
IncludeDir["Entt"] = "vendor/Entt"
IncludeDir["AssetUtils"] = "vendor/AssetUtils"

-- Projects
group "Dependencies"
    include "OpenGLCore/vendor/GLFW"
    include "OpenGLCore/vendor/Glad"
    include "OpenGLCore/vendor/imgui"
    include "VoxelEngine/vendor/JoltPhysics"
group ""

include "OpenGLCore"
include "VoxelEngine"
include "Sandbox"