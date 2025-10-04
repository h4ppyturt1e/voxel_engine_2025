# Voxel Engine 2025 - Architecture Overview

## Project Structure

The engine follows a modular architecture with clear separation of concerns:

```
src/
├── app/           # Application entry point and initialization
├── camera/        # Camera system and movement controls
├── config/        # Configuration management and file handling
├── core/          # Core utilities (logging, math, common types)
├── input/         # Input system with action-based mapping
├── mesh/          # Mesh generation and data structures
├── render/        # OpenGL rendering, raycast, and graphics
└── voxel/         # Voxel storage, chunks, and world management
```

## Core Systems

### 1. Input System (`src/input/`)

**Current Implementation:**
- `input_manager.hpp/cpp` - Central input handling with action mapping
- `key_constants.hpp/cpp` - Key name to GLFW key code conversion

**Features:**
- Action-based input mapping (e.g., "MoveForward" instead of "W")
- Hot reload support for `input.ini` changes
- Context-sensitive input (Game/Menu states)
- Configurable mouse sensitivity

**Configuration:**
```ini
[actions]
MoveForward=W
MoveBackward=S
MoveLeft=A
MoveRight=D
MoveUp=SPACE
MoveDown=CTRL
FastMovement=SHIFT
BreakBlock=MOUSE_LEFT
PlaceBlock=MOUSE_RIGHT

[settings]
mouse_sensitivity=0.01
```

### 2. Configuration System (`src/config/`)

**Current Implementation:**
- `config.hpp/cpp` - Engine configuration loading
- `config_manager.hpp/cpp` - Runtime config file management

**Features:**
- Automatic config file copying from `default_config/` to runtime location
- Full absolute path logging for file operations
- Support for multiple config files (`engine.ini`, `input.ini`)
- Default configuration preservation

### 3. Mesh System (`src/mesh/`)

**Current Implementation:**
- `mesh.hpp/cpp` - Mesh data structures (Vertex, Mesh)
- `greedy_mesher.hpp/cpp` - Greedy meshing algorithm

**Features:**
- Efficient face culling (only visible faces)
- Greedy meshing for reduced triangle count
- Clean separation of data and generation logic

### 4. Rendering System (`src/render/`)

**Current Implementation:**
- `gl_app.hpp/cpp` - Main OpenGL application and demo
- `raycast.hpp/cpp` - Ray-voxel intersection for block editing
- `glad/` - OpenGL function loader

**Features:**
- Immediate mode OpenGL rendering
- Raycast-based block selection and placement
- Camera controls with FPS-independent movement
- Debug overlay and wireframe mode

### 5. Voxel System (`src/voxel/`)

**Current Implementation:**
- `voxel.hpp/cpp` - Voxel data structure
- `chunk.hpp/cpp` - Chunk storage and serialization
- `world.hpp/cpp` - World management
- `world_manager.hpp/cpp` - Chunk loading and view distance

**Features:**
- Chunk-based world partitioning
- Voxel type system (Air, Dirt, etc.)
- File-based chunk persistence
- View distance culling

## Configuration Management

### Runtime Config Location
All config files are placed in `build/bin/Release/config/` alongside the executable:
- `engine.ini` - Engine settings (chunk size, logging, graphics)
- `input.ini` - Input bindings and mouse sensitivity

### Default Config Source
Default configurations are stored in `default_config/` and copied to runtime location on first run. The defaults are never modified by the application.

### Hot Reload Support
Input bindings can be changed in `input.ini` and are automatically reloaded without restarting the application.

## Input System Architecture

### Action-Based Mapping
Instead of hardcoding key checks, the system uses abstract actions:
```cpp
enum class Action {
    MoveForward, MoveBackward, MoveLeft, MoveRight,
    MoveUp, MoveDown, FastMovement,
    LookUp, LookDown, LookLeft, LookRight,
    ToggleMenu, ToggleDebug, ToggleWireframe,
    BreakBlock, PlaceBlock
};
```

### Context-Sensitive Input
Different input contexts allow for different key mappings:
```cpp
enum class InputContext {
    Game,    // Normal gameplay controls
    Menu     // Menu navigation controls
};
```

### Key Name Mapping
Human-readable key names in config files are converted to GLFW key codes:
```cpp
// Config file uses: MoveForward=W
// System converts: W -> GLFW_KEY_W (87)
```

## Logging System

### Features
- **Multi-level logging**: Debug, Info, Warn, Error
- **File rotation**: Keeps latest 50 log files
- **Full path logging**: All file operations show absolute paths
- **Console output**: Real-time logs with timestamps
- **Session separation**: Clear session boundaries in log files

### Example Output
```
[2025-10-04 02:27:11.602] [INFO] Saved chunk to C:\Users\ASUS\Documents\GitHub\voxel_engine_2025\build\bin\Release\chunk_0_0.vxl
[2025-10-04 02:27:11.603] [INFO] Mesh: vertices=384, indices=576
```

## Build System

### CMake Configuration
- **Modular libraries**: Each module builds as a separate static library
- **External dependencies**: GLFW fetched via FetchContent
- **Platform support**: Windows (Visual Studio), Linux (GCC/Clang)
- **Optional components**: OpenGL rendering can be disabled

### Build Options
```cmake
option(VOXEL_WITH_GL "Enable OpenGL/GLFW renderer" OFF)
```

## Future Enhancements

### Planned Features
1. **UI System**: Overlay framework for menus and HUD
2. **Settings Menu**: Visual configuration interface
3. **Key Binding UI**: Interactive key remapping
4. **Shader Pipeline**: VBO/IBO rendering with shaders
5. **Multi-chunk Support**: Seamless world streaming

### Architecture Principles
- **Separation of Concerns**: Each module has a single responsibility
- **Dependency Injection**: Systems communicate through well-defined interfaces
- **Configuration-Driven**: Behavior controlled by config files
- **Error Handling**: Comprehensive logging and graceful degradation
- **Performance**: Efficient algorithms and data structures

## Development Guidelines

### Code Organization
- Use namespaces to separate modules
- Keep headers minimal with forward declarations
- Implement in separate .cpp files
- Follow RAII principles

### Error Handling
- Log all file operations with full paths
- Provide meaningful error messages
- Use exceptions for unrecoverable errors
- Validate inputs and configurations

### Testing
- Build and run after each significant change
- Test configuration changes and hot reload
- Verify cross-platform compatibility
- Check memory usage and performance