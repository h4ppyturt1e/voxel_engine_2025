add_library(voxel STATIC
    voxel.hpp
    chunk.hpp
    world.hpp
    world_manager.hpp
    voxel.cpp
    chunk.cpp
    world.cpp
    world_manager.cpp
)

target_include_directories(voxel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(voxel PUBLIC core)


