add_library(core STATIC
    logging.cpp
    math.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


