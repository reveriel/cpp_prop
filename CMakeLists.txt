# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(cpp_prop CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This command is key: it tells CMake to generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add an executable. CMake needs at least one target to generate commands.
# We'll create a dummy main.cpp for this.
add_executable(main main.cpp)

# Add the current directory to the include path to find constructive_logic.h
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add the prover executable
add_executable(prover prover.cpp)

# Add the enhanced_prover executable for demonstrating classical logic theorems
add_executable(enhanced_prover enhanced_prover.cpp)

# You can add include directories here if needed in the future
# target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Testing --- #
# Enable testing for this project
enable_testing()

# Include the Google Test suite
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Make the Google Test content available
FetchContent_MakeAvailable(googletest)

# Add the tests subdirectory
add_subdirectory(tests)
