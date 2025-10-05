# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- **UI System with Dear ImGui Integration**
  - UIManager for centralized overlay management
  - Overlay base class with HUD, Settings, and KeyBindings menus
  - Modal overlay behavior with proper input blocking
  - Real-time FPS counter and transparent crosshair in HUD
  - Settings menu with Graphics, Audio, UI, and Controls tabs (Apply button non-functional)
  - Key bindings menu with placeholder remapping functionality
- **Advanced Game State Management**
  - True game pause system with Running/Paused states
  - Automatic pause/resume when opening/closing modal menus
  - Visual pause indicators in window title and HUD
  - Game state logging for debugging
- **Enhanced Configuration System**
  - Extended config system for Graphics, UI, and Audio settings
  - Mouse sensitivity, VSync, resolution, and quality settings
  - Audio volume controls (master, SFX, music) and device selection
  - UI theme selection (dark/light) and scaling options
- **Improved User Experience**
  - Centered modal menus for professional appearance
  - Consistent ESC and Close button behavior for menu management
  - Fixed FPS counter showing real-time values instead of fixed rate
  - Clean crosshair rendering without background artifacts
- Action-based input system with configurable key bindings
- Hot reload support for input configuration changes
- Context-sensitive input (Game/Menu states)
- ConfigManager for runtime configuration file management
- Full file path logging for better debugging
- Greedy meshing algorithm for efficient mesh generation
- Raycast system for block selection and editing

### Changed
- **UI System Integration**
  - Integrated Dear ImGui library with GLFW and OpenGL backends
  - Replaced hardcoded crosshair with ImGui-based transparent crosshair
  - Centralized UI rendering through UIManager instead of direct ImGui calls
  - Updated render loop to support UI overlay management
- **Game State Architecture**
  - Implemented proper game state management (Running/Paused)
  - Modified input handling to respect game pause state
  - Updated FPS calculation to pause when game is paused
  - Enhanced window title to show pause state
- **Configuration System**
  - Extended config system to support UI, Graphics, and Audio settings
  - Added new configuration sections for comprehensive game settings
  - Updated default configuration files with new setting categories
- Refactored input system to use InputManager instead of direct GLFW calls
- Moved mesh generation logic to dedicated GreedyMesher class
- Extracted raycast functionality to separate module
- Updated configuration system to use runtime config directory
- Improved logging with absolute file paths

### Removed
- Duplicate InputSystem classes
- Stub Renderer and Pipeline classes
- Hardcoded key mappings in favor of configurable system

### Fixed
- **UI System Issues**
  - Fixed FPS counter showing fixed value (62.5) instead of real-time FPS
  - Fixed black rectangle crosshair by implementing transparent ImGui crosshair
  - Fixed ESC key not properly opening/closing settings menu
  - Fixed segmentation fault on application shutdown by centralizing ImGui lifecycle
  - Fixed Close button in menus not unpausing the game
  - Fixed duplicate crosshair rendering (removed hardcoded OpenGL crosshair)
- **Game State Issues**
  - Fixed game not truly pausing when menus are open (now properly freezes world updates)
  - Fixed inconsistent pause behavior between ESC and Close button
  - Fixed mouse tracking continuing when game is paused
  - Fixed FPS calculation continuing when game is paused
- **Technical Issues**
  - Fixed ImGui initialization/shutdown being called multiple times
  - Fixed overlay management not properly handling modal behavior
  - Fixed menu centering issues (now properly centered on screen)
- FPS-independent movement using delta time
- Chunk file saving to correct executable directory
- Config file path resolution issues

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