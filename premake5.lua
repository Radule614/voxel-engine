workspace "VoxelEngine"
    architecture "x64"
    startproject "VoxelEngine"

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

-- Include directories relative to OpenGLCore
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"

-- Projects
group "Dependencies"
    includeexternal "OpenGLCore/vendor/GLFW"
    includeexternal "OpenGLCore/vendor/Glad"
    includeexternal "OpenGLCore/vendor/imgui"
group ""

includeexternal "OpenGLCore"
include "VoxelEngine"