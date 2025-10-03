add_library(config STATIC
    config.hpp
    config.cpp
)

target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


