cmake_minimum_required(VERSION 3.15)
project(GMU_fluid_simulation_cellular_automata)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fconcepts")
include(FetchContent)

add_compile_definitions(SRC_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL2/lib/cmake/SDL2)

find_package(SDL2 CONFIG REQUIRED)

FetchContent_Declare(SDL2CPP GIT_REPOSITORY https://github.com/dormon/SDL2CPP.git)
FetchContent_Declare(geGL GIT_REPOSITORY https://github.com/dormon/geGL.git)


FetchContent_GetProperties(SDL2CPP)
if (NOT SDL2CPP_POPULATED)
    FetchContent_Populate(SDL2CPP)
    add_subdirectory(${sdl2cpp_SOURCE_DIR} ${sdl2cpp_BINARY_DIR})
endif ()

FetchContent_GetProperties(geGL)
if (NOT geGL_POPULATED)
    FetchContent_Populate(geGL)
    add_subdirectory(${gegl_SOURCE_DIR} ${gegl_BINARY_DIR})
endif ()

include_directories(misc)
include_directories(Graphics)
include_directories(Graphics/Renderers/Simulators)
include_directories(ThirdParty)
include_directories(Simulation)
include_directories(Maps)

file(GLOB_RECURSE misc_src "misc/*.cpp")
file(GLOB_RECURSE Graphics_src "Graphics/*.cpp")
file(GLOB_RECURSE Simulators_src "Graphics/Renderers/Simulators/*.cpp")
file(GLOB_RECURSE imgui_src "ThirdParty/imgui/*.cpp")
file(GLOB_RECURSE Simulation_src "Simulation/*.cpp")
file(GLOB_RECURSE fmt_src "ThirdParty/fmt/*.cc")
file(GLOB_RECURSE Maps "Maps/*.cpp")

add_compile_definitions(
        GLM_ENABLE_EXPERIMENTAL
        TINYOBJLOADER_IMPLEMENTATION
        )

add_executable(GMU_fluid_simulation_cellular_automata ${misc_src} ${Graphics_src} ${Simulators_src}
        ${imgui_src} ${Maps}
        ${fmt_src}
        ${Simulation_src} main.cpp ui.cpp ui.h)

target_link_libraries(GMU_fluid_simulation_cellular_automata geGL::geGL SDL2CPP::SDL2CPP
        )