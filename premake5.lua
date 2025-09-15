---@diagnostic disable: undefined-global
require "cmake"  -- enable Premake's CMake exporter

workspace "Renderer"
    configurations { "Debug", "Profile", "Dist" }
    architecture "x64"
    location "build"
    filter "action:cmake"
        location "."
    filter {}

-- Options
newoption {
    trigger = "scene",
    value = "SCENE_NAME",
    description = "Single .cpp file to build from src/render-projects"
}
local configFile = "config/build_config.json"
local config = dofile(configFile)
if not config then error("Failed to read/parse config file: " .. configFile) end

local dialect = config.cpp_dialect or "C++23"
local project_dirs = config.projects_dir or "src/render-projects"

local selectedScene = _OPTIONS["scene"]
if not selectedScene then error("Missing --scene. Usage: premake5 --scene=MyScene cmake|gmake2") end

local scenePath = path.join(project_dirs, selectedScene .. ".cpp")
if not os.isfile(scenePath) then error("Scene source not found: " .. scenePath) end

startproject(selectedScene)

local outdir = selectedScene .. "/build-%{cfg.architecture}"

local inc = {
    ".",
    "src",
    "src/isbell",
    "src/isbell/utils",
    "src/isbell/engine",
    "src/isbell/geometry",
    "src/isbell/physics",
    "src/isbell/file-management",
    "src/isbell/openglAPI",
    "src/isbell/include",

    "src/render-projects/",
    "src/tests",

    "external/glew-2.1.0/include",
    "external/glfw-3.4/include",
    "external/glm-0.9.1.7",
    "external/spdlog-1.x/include",
}

-- Use a more readable format for Windows system libraries
local winlibs = {
    "opengl32",
    "gdi32",
    "user32",
    "shell32",
    "ole32",
    "uuid",
    "winmm",
    "imm32"
}

-----------------------
-- Engine static library
-----------------------
project "engine"
    location "build/engine-build"
    kind "StaticLib"
    language "C++"
    cppdialect(dialect)
    staticruntime "off"
    targetdir ("build/engine-build/%{cfg.architecture}/bin")
    objdir    ("build/engine-build/%{cfg.architecture}/obj")

    files {
        "src/isbell/**.hpp",
        "src/isbell/**.cpp",
        "external/glew-2.1.0/src/glew.c",
        "external/glfw-3.4/src/**.c",
        "external/glm-0.9.7.1/glm/**.hpp",

    }
    removefiles { "src/isbell/**_dep.**" }


    includedirs(inc)

    filter "system:windows"
        systemversion "latest"
        defines { "_WINDOWS", "WIN32_LEAN_AND_MEAN", "NOMINMAX", "GLEW_STATIC", "_GLFW_WIN32" }
    filter {}

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
    filter "configurations:Profile"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "On"
        optimize "Speed"
        linktimeoptimization "On"
    filter "configurations:Dist"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "Off"
        optimize "Speed"
        linktimeoptimization "On"
    filter {}



-----------------------
-- Scene application (Console)
-----------------------
project(selectedScene)
    location ("build/" .. selectedScene)
    language "C++"
    cppdialect(dialect)
    staticruntime "off"
    kind "ConsoleApp"

    links { "engine" } -- Link the static library

    filter "system:windows"
        systemversion "latest"
        defines { "_WINDOWS", "WIN32_LEAN_AND_MEAN", "NOMINMAX", "GLEW_STATIC", "_GLFW_WIN32" }
        links(winlibs)
    filter {}

    targetdir ("build/" .. selectedScene .. "/bin/build-%{cfg.architecture}")
    objdir    ("build/" .. selectedScene .. "/obj/build-%{cfg.architecture}")

    files { scenePath }
    includedirs(inc)

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
    filter "configurations:Profile"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "On"
        optimize "Speed"
        linktimeoptimization "On"
    filter "configurations:Dist"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "Off"
        optimize "Speed"
        linktimeoptimization "On"
    filter {}


project "unitTests"
    location ("build/tests")
    language "C++"
    cppdialect(dialect)
    staticruntime "off"
    kind "ConsoleApp"

    links { "engine" }

    filter "system:windows"
        systemversion "latest"
        defines { "_WINDOWS", "WIN32_LEAN_AND_MEAN", "NOMINMAX", "GLEW_STATIC", "_GLFW_WIN32" }
        links(winlibs)
    filter {}

    targetdir ("build/tests/bin/build-%{cfg.architecture}")
    objdir    ("build/tests/obj/build-%{cfg.architecture}")

    files { "src/tests/runTests.cpp", "src/tests/**.hpp"}

    includedirs(inc)

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
    filter "configurations:Profile"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "On"
        optimize "Speed"
        linktimeoptimization "On"
    filter "configurations:Dist"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "Off"
        optimize "Speed"
        linktimeoptimization "On"
    filter {}
