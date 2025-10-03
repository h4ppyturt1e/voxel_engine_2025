cmake_minimum_required(VERSION 3.20)

project(voxel_engine_2025 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VOXEL_BUILD_WARNINGS "Enable extra compiler warnings" ON)

if(MSVC)
    add_compile_options(/W4)
else()
    if(VOXEL_BUILD_WARNINGS)
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

add_subdirectory(src/core)
add_subdirectory(src/config)
add_subdirectory(src/voxel)
add_subdirectory(src/mesh)
add_subdirectory(src/render)
add_subdirectory(src/camera)
add_subdirectory(src/input)
add_subdirectory(src/app)


