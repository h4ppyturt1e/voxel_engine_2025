add_library(mesh STATIC
    mesh.hpp
    greedy_mesher.hpp
    mesh.cpp
)

target_include_directories(mesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mesh PUBLIC voxel core)


