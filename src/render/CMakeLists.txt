add_library(render STATIC
    renderer.hpp
    pipeline.hpp
    renderer.cpp
    pipeline.cpp
)

target_include_directories(render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render PUBLIC mesh core)


