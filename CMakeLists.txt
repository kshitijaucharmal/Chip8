# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(Redchip VERSION 1.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add SDL (but disable shared builds for web)
set(SDL_SHARED OFF CACHE BOOL "Build shared SDL library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static SDL library" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Disable SDL test programs" FORCE)

# SDL
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

# Add source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Create an executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries (example, can remove if not needed)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
