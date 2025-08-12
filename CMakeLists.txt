cmake_minimum_required(VERSION 3.25.0)
project(csnake LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

set(RAYLIB_DIR "/usr/lib/raylib-5.5_linux_amd64")

include_directories(${RAYLIB_DIR}/include)
link_directories(${RAYLIB_DIR}/lib)

add_executable(csnake main.c)

target_link_libraries(csnake raylib m dl pthread)
