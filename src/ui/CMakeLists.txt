add_library(ui STATIC
    ui_manager.cpp
    overlay.cpp
    hud.cpp
    settings_menu.cpp
    key_bindings_menu.cpp
)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ui PUBLIC config core input)

option(VOXEL_WITH_GL "Enable OpenGL/GLFW renderer" OFF)
if(VOXEL_WITH_GL)
    target_link_libraries(ui PUBLIC imgui)
    target_compile_definitions(ui PUBLIC VOXEL_WITH_GL=1)
endif()
