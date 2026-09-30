cmake_minimum_required(VERSION 3.16)
project(chip8 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- SDL2 (vendored, builds via its own CMakeLists.txt) ---
set(SDL_SHARED OFF CACHE BOOL "" FORCE)   # build static, no runtime .so to ship
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/SDL2)

# --- glad (vendored, plain generated source, build as a tiny static lib) ---
add_library(glad STATIC vendor/glad/src/gl.c)
target_include_directories(glad PUBLIC vendor/glad/include)

find_package(OpenGL REQUIRED)

add_executable(chip8 
    src/Chip8.cpp
    src/Client.cpp
    src/main.cpp)

target_link_libraries(chip8
    PRIVATE
        SDL2::SDL2main
        SDL2::SDL2-static
        glad
        OpenGL::GL
)
