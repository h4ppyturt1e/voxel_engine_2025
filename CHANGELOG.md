# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-10-03

### Added
- Initial voxel engine with modular architecture
- Voxel storage and world partitioning system
- Mesh generation with visible-face culling
- OpenGL rendering pipeline with immediate mode
- Camera controls (WASD movement, mouse look)
- Interactive voxel editing (left-click remove, right-click place)
- Configurable chunk dimensions via `engine.ini`
- Logging system with file rotation and console output
- F3 debug overlay (camera position, hit voxel, FPS counter)
- F4 mouse lock/unlock toggle
- VSync configuration via `engine.ini`
- Semantic versioning system with `VERSION` file
- Stable release system with automated packaging
- Windows build support with Visual Studio 2022
- Comprehensive documentation and README

### Technical Details
- **Architecture**: Modular C++ design with separate modules for voxel, mesh, render, camera, input, and config
- **Build System**: CMake with external dependency management (GLFW)
- **Platforms**: Windows (recommended), Linux/WSL (alternative)
- **Graphics**: OpenGL with GLAD loader
- **Configuration**: INI-based config system
- **Logging**: Multi-level logging with file rotation (50 files max)
- **Versioning**: Semantic versioning (MAJOR.MINOR.PATCH)

### Controls
- **Mouse**: Look around (inverted Y)
- **WASD**: Free-fly movement relative to facing direction
- **Left Click**: Remove voxel (origin block protected)
- **Right Click**: Place voxel on highlighted face
- **F**: Toggle wireframe mode
- **R**: Recenter camera to world origin
- **F3**: Toggle debug information overlay
- **F4**: Toggle mouse lock (cursor capture)

### Configuration
```ini
# Voxel Engine 2025 Configuration
chunk.size_x=8
chunk.size_y=8
chunk.size_z=8
log.level=debug
log.file=logs/engine.log

[graphics]
vsync=true
```

### Files
- **Executable**: `build/bin/Release/voxel_app.exe` (Windows)
- **Config**: `engine.ini`
- **Version**: `VERSION` (1.0.0)
- **Stable Release**: `stable_releases/voxel_engine_1.0.0/`
