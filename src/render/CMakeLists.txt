add_library(render STATIC
    gl_app.cpp
    gl_app.hpp
    raycast.cpp
    raycast.hpp
)

target_include_directories(render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render PUBLIC mesh core)

option(VOXEL_WITH_GL "Enable OpenGL/GLFW renderer" OFF)
if(VOXEL_WITH_GL)
    include(FetchContent)
    FetchContent_Declare(
      glfw
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG 3.3.9
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)

    add_library(glad STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c
    )
    target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad)
    find_package(OpenGL REQUIRED)
    target_link_libraries(render PUBLIC glfw glad OpenGL::GL)
    target_compile_definitions(render PUBLIC VOXEL_WITH_GL=1)
endif()


