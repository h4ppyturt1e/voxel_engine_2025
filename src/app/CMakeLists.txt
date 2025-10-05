add_executable(voxel_app
    main.cpp
)

target_link_libraries(voxel_app PRIVATE
    core
    config
    voxel
    mesh
    render
    input
    ui
)

target_compile_definitions(voxel_app PRIVATE
    $<$<CONFIG:Debug>:VOXEL_DEBUG>
)

set_target_properties(voxel_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_definitions(voxel_app PRIVATE $<$<BOOL:${VOXEL_WITH_GL}>:VOXEL_WITH_GL=1>)


