workspace "ChessAI"
	configurations { "Debug", "Release" }
	platforms { "Win32", "Win64" }

project "ChessAI"

	kind "ConsoleApp"
	language "C++"
	targetdir "out/%{cfg.buildcfg}"

	files {
		"src/**.h",
		"src/**.cpp",
		"premake5.lua",
		"lib/glm/glm/**.hpp",
		"lib/glm/glm/**.inl",
		"lib/imgui/*.h",
		"lib/imgui/*.cpp",
		"lib/imgui/backends/imgui_impl_opengl3.*",
		"lib/imgui/backends/imgui_impl_glfw.*"
	}


	includedirs {
		"lib/imgui",
		"lib/imgui/backends",
		"lib/glm",
		"lib/glew/include",
		"lib/glfw/include"
	}

	links {"opengl32"}

	

	filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

	filter "platforms:Win32"
	  system "Windows"
	  architecture "x86"
	  libdirs { "lib/glew/lib/Release/Win32" }
	  links {"glew32.lib"}

	filter "platforms:Win64"
	  system "Windows"
	  architecture "x86_64"
	  libdirs { "lib/glew/lib/Release/x64" }
	  links {"glew32.lib"}