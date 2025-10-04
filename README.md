# Voxel Engine 2025

A modern, modular C++ voxel engine with clean architecture and configurable input system.

## Features

- **Modular Architecture**: Clean separation between voxel, mesh, render, input, and config systems
- **Action-Based Input**: Configurable key bindings with hot reload support
- **Greedy Meshing**: Efficient mesh generation with face culling
- **OpenGL Rendering**: Modern graphics pipeline with raycast-based block editing
- **Configuration System**: Runtime config files with defaults and user overrides
- **Comprehensive Logging**: Full file path logging with rotation and multiple levels

## Quick Start

### Prerequisites
- Visual Studio 2022 Community (with "Desktop development with C++" workload)
- CMake 3.16+

### Build & Run
```cmd
git clone <repo-url>
cd voxel_engine_2025
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 -DVOXEL_WITH_GL=ON ..
cmake --build . --config Release
.\bin\Release\voxel_app.exe
```

## Architecture

```
src/
├── app/           # Application entry point
├── camera/        # Camera system and controls
├── config/        # Configuration management
├── core/          # Core utilities (logging, math)
├── input/         # Input system with action mapping
├── mesh/          # Mesh generation (greedy meshing)
├── render/        # OpenGL rendering and raycast
└── voxel/         # Voxel storage and world management
```

## Controls

All controls are configurable via `input.ini`:

### Movement
- **W/A/S/D**: Move forward/left/backward/right
- **Space/Ctrl**: Move up/down
- **Shift**: Fast movement
- **Mouse**: Look around

### Block Editing
- **Left Click**: Remove block
- **Right Click**: Place block

### Debug & Settings
- **F**: Toggle wireframe
- **F3**: Toggle debug overlay
- **F4**: Toggle mouse lock
- **F5**: Toggle VSync
- **R**: Recenter camera
- **Escape**: Toggle menu (planned)

## Configuration

### Runtime Config Files
Config files are automatically copied to `build/bin/Release/config/` on first run:

#### `engine.ini` - Engine Settings
```ini
# Chunk dimensions
chunk.size_x=8
chunk.size_y=8
chunk.size_z=8

# Logging
log.level=debug
log.file=logs/engine.log

# Graphics
vsync=false
```

#### `input.ini` - Input Bindings
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

### Hot Reload
Input bindings can be changed in `input.ini` and will be reloaded automatically without restarting the application.

## Build Options

### Windows (Recommended)
```cmd
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 -DVOXEL_WITH_GL=ON ..
cmake --build . --config Release
.\bin\Release\voxel_app.exe
```

### Linux/WSL
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DVOXEL_WITH_GL=ON ..
cmake --build . --config Release
./bin/voxel_app
```

## Logging

- **Console Output**: Real-time logs with timestamps
- **File Logging**: Automatic rotation (keeps latest 50 files)
- **Full Paths**: All file operations show absolute paths
- **Levels**: Debug, Info, Warn, Error

Example log output:
```
[2025-10-04 02:27:11.602] [INFO] Saved chunk to C:\Users\ASUS\Documents\GitHub\voxel_engine_2025\build\bin\Release\chunk_0_0.vxl
```

## Development

### Key Components

- **InputManager**: Central input handling with action mapping
- **ConfigManager**: Runtime config file management
- **GreedyMesher**: Efficient mesh generation
- **Raycast**: Block selection and editing
- **Voxel System**: Chunk-based world storage

### Code Style
- Modern C++ with RAII and smart pointers
- Namespace organization by module
- Comprehensive error handling and logging
- Separation of concerns between modules

## Version History

See [CHANGELOG.md](CHANGELOG.md) for detailed version history.

## License

This project is licensed under the MIT License - see the LICENSE file for details.