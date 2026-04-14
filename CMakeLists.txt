cmake_minimum_required(VERSION 3.16)

project(IZG2026)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON )
add_subdirectory(libs/SDL-release-3.4.4)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

add_custom_target(run ./${PROJECT_NAME})
