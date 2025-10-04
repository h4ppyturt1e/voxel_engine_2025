add_library(config STATIC
    config.hpp
    config.cpp
    config_manager.hpp
    config_manager.cpp
)

target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


