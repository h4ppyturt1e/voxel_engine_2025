add_executable(voxel_app
    main.cpp
)

target_link_libraries(voxel_app PRIVATE
    core
    voxel
    mesh
    render
    camera
    input
)

target_compile_definitions(voxel_app PRIVATE
    $<$<CONFIG:Debug>:VOXEL_DEBUG>
)

set_target_properties(voxel_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


