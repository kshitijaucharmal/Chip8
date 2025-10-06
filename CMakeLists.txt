# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(Redchip VERSION 1.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB SOURCES CONFIGURE_DEPENDS src/*)

# SDL
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

# Create an executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries (example, can remove if not needed)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
