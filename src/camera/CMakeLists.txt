add_library(camera STATIC
    camera.hpp
    controller.hpp
    camera.cpp
    controller.cpp
)

target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(camera PUBLIC core)


