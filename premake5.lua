workspace "ChessAI"
	configurations { "Debug", "Release" }
	platforms { "Win32", "x64" }

project "ChessAI"

	kind "ConsoleApp"
	language "C++"

	outputdir = "out/%{cfg.buildcfg}"
	
	targetdir (outputdir)

	files {
		"src/**.h",
		"src/**.cpp",
		"premake5.lua"
	}


	includedirs {
		"lib/ImGui",
		"lib/ImGui/backends",
		"lib/glm",
		"lib/glew/include",
		"lib/glfw/include"
	}

	links { "opengl32", "ImGui", "GLFW", "glew32s" }

	defines { "GLEW_STATIC" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

    filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	filter "platforms:Win32"
		system "Windows"
		architecture "x86"


	filter "platforms:Win64"
		system "Windows"
		architecture "x86_64"

	filter "architecture:x86"
		libdirs { "lib/glew/lib/Release/Win32" }

	filter "architecture:x64"
		libdirs { "lib/glew/lib/Release/x64" }
	filter { }

	include "lib/ImGui/Build_ImGui.lua"
	include "lib/glfw"