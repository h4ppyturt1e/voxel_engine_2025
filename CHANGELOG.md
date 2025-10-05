# Changelog

### Documentation rule
- Last documented commit: 969c07c
- When updating docs or changelog, always include the exact last documented commit hash at the top and update it.

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Last documented commit: 969c07c

### Added
- Build time displayed in HUD bottom-right as faded text (858ffd5)
- Runtime VSync application via UI and F5 toggle (63a4727, 858ffd5)
- Cursor lock management through UIManager; menu shows cursor, closes re-lock (969c07c)

### Changed
- Sensitivity unified to use `engine.ini` as source of truth (63a4727)
- INI cleanup: removed overlap; moved settings from `input.ini` to `engine.ini` (63a4727)

### Removed
- Audio settings and UI (858ffd5)
- Dead code: camera module, unused enums/methods/properties (d6f4b5a, 2c315f6)

### Fixed
- Settings menu now applies only on Apply; temp values reset on reopen (63a4727)
- Black background artifact replaced with overlay text for build time (858ffd5)
- VSync state persisted across toggles and reflected in config (63a4727, 858ffd5)

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
- **ESC**: Open/close settings menu (pauses game)
- **UI Controls**: Settings menu with Graphics, Audio, UI, and Controls tabs

### Configuration
```ini
# Voxel Engine 2025 Configuration
chunk.size_x=8
chunk.size_y=8
chunk.size_z=8
log.level=debug
log.file=logs/engine.log

[graphics]
vsync=false
graphics.resolution_width=800
graphics.resolution_height=600
graphics.quality=medium

[ui]
ui.mouse_sensitivity=0.01
ui.theme=dark
ui.scale=1.0

[audio]
audio.master_volume=1.0
audio.sfx_volume=1.0
audio.music_volume=1.0
audio.device=default
```

### Files
- **Executable**: `build/bin/Release/voxel_app.exe` (Windows)
- **Config**: `engine.ini`
- **Version**: `VERSION` (1.0.0)
- **Stable Release**: `stable_releases/voxel_engine_1.0.0/`