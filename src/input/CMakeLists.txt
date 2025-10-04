add_library(input STATIC
    input_manager.hpp
    input_manager.cpp
    key_constants.hpp
    key_constants.cpp
)

target_include_directories(input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(input PUBLIC core)


