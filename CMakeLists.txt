# CMakeList.txt : CMake project for chess_ai, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("chess_ai")

# Add source to this project's executable.
add_executable (chess_ai "chess_ai.cpp" "chess_ai.h" "game.cpp" "game.h" "piece.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chess_ai PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
