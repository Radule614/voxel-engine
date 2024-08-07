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
    include "OpenGLCore/vendor/GLFW"
    include "OpenGLCore/vendor/Glad"
    include "OpenGLCore/vendor/imgui"
    include "VoxelEngine/vendor/JoltPhysics"
group ""

include "OpenGLCore"
include "VoxelEngine"