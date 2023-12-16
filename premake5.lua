workspace "ChessAI"
	configurations { "Debug", "Release"}

project "ChessAI"

	kind "ConsoleApp"
	language "C++"
	targetdir "out/%{cfg.buildcfg}"

	files {"src/**.h", "src/**.cpp", "premake5.lua", "lib/glm/glm/**.hpp", "lib/glm/glm/**.inl"}

	includedirs {
		"lib/imgui",
		"lib/imgui/backends",
		"lib/glm",
		"lib/glew/include",
		"lib/glfw/include"
	}

	filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"