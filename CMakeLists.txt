# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(Chip8 VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB SOURCES CONFIGURE_DEPENDS src/*)

# Create an executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})


# Link libraries (example, can remove if not needed)
# target_link_libraries(${PROJECT_NAME} PRIVATE some_library)
