# Dead Code Analysis Report

## Overview
This report documents all instances of dead code found in the voxel engine codebase - code that is defined but never used. Each item is categorized by type and location.

## Summary Statistics
- **Unused Classes**: 2
- **Unused Functions/Methods**: 18
- **Unused Enums/Values**: 4
- **Unused Action Types**: 5
- **Total Dead Code Items**: 29

---

## Detailed Findings

### 1. Unused Classes

#### Camera Module - Complete Module Unused
- **Location**: `src/camera/`
- **Classes**: 
  - `camera::Camera` (camera.hpp:5)
  - `camera::Controller` (controller.hpp:5)
- **Methods**:
  - `Camera::setPerspective()` (camera.cpp:5)
  - `Controller::update()` (controller.cpp:5)
- **Impact**: Entire camera module is defined but never instantiated or used

### 2. Unused Core Functions/Methods

#### Logging Module
- **Function**: `Logger::level()` (logging.hpp:41, logging.cpp:81)
  - Returns current log level but never called
- **Macros**: All VOX_LOG_* macros (logging.hpp:56-59)
  - `VOX_LOG_DEBUG`, `VOX_LOG_INFO`, `VOX_LOG_WARN`, `VOX_LOG_ERROR`
  - Defined but never used anywhere in codebase

#### Math Module
- **Function**: `Mat4::identity()` (math.hpp:13, math.cpp:5)
  - Only called internally within `lookAt()` function but never used externally

### 3. Unused Input System Components

#### Unused Action Types
The following Action enum values are defined but never checked/used:
- **Location**: `src/input/input_manager.hpp:18`
- **Values**:
  - `Action::LookUp`
  - `Action::LookDown` 
  - `Action::LookLeft`
  - `Action::LookRight`
  - `Action::ToggleMenu`

#### Unused Input Manager Methods
- **Methods** (all in `src/input/input_manager.cpp`):
  - `isActionJustPressed()` (line 126) - defined but never called
  - `isActionJustReleased()` (line 138) - defined but never called
  - `getMouseSensitivity()` (line 162) - defined but never called
  - `getCurrentContext()` (line 190) - defined but never called
  - `resetToDefaults()` (line 179) - defined but never called
  - `setKeyMapping()` (line 166) - defined but never called 
  - `getKeyMapping()` (line 170) - defined but never called
  - `getActionName()` (line 175) - defined but never called
  - `setContext()` (line 186) - defined but never called
  - `setKeyMappingForContext()` (line 194) - defined but never called
  - `isConfigFileModified()` (line 288) - only used internally in update() but provides no external value
  - `reloadConfig()` (line 273) - defined but never called

### 4. Unused Voxel System Components

#### World Management
- **Function**: `World::unloadChunk()` (world.cpp:20)
  - Defined but never called, chunks are created but never explicitly unloaded

#### Block Types
- **Location**: `src/voxel/voxel.hpp:7-13`
- **Unused Values**:
  - `BlockType::Stone`
  - `BlockType::Grass` 
  - `BlockType::Sand`
- **Note**: Only `Air` and `Dirt` are actually used in the codebase

#### Voxel Properties
- **Property**: `Voxel::light` (voxel.hpp:17)
  - Defined and serialized/deserialized but never used for any lighting calculations

### 5. Unused Render Components

#### Glad Integration
- **Function**: `glad_placeholder()` (glad.c:3)
  - Placeholder function that exists only to satisfy linker, never called

### 6. Configuration System Issues

#### Input Context System
- **Enum**: `InputContext` (input_manager.hpp:9-13)
  - `InputContext::Menu` - defined but never used
  - Context switching system implemented but Menu context never activated
  - Only `InputContext::Game` is used

---

## Recommendations for Cleanup

### High Priority (Safe to Remove)
1. **Camera Module**: Entire `src/camera/` directory can be removed
2. **VOX_LOG Macros**: Remove unused logging macros from logging.hpp
3. **Unused Block Types**: Remove `Stone`, `Grass`, `Sand` from BlockType enum
4. **Unused Actions**: Remove `LookUp`, `LookDown`, `LookLeft`, `LookRight`, `ToggleMenu` from Action enum
5. **glad_placeholder()**: Remove from glad.c

### Medium Priority (Review Before Removal)
1. **Input Manager Methods**: Many getter/setter methods that may be intended for future use
2. **World::unloadChunk()**: May be needed for memory management in larger worlds
3. **Voxel::light**: May be intended for future lighting system
4. **InputContext::Menu**: May be planned for future UI system

### Low Priority (Keep for Future Development)
1. **Mat4::identity()**: Utility function that may be needed later
2. **Input hot-reload system**: Methods like `isConfigFileModified()` and `reloadConfig()`

---

## Prompt for Fixing Dead Code

Use this prompt to systematically remove dead code:

```
Remove dead code from the voxel engine codebase in this order:

1. DELETE entire camera module (src/camera/ directory) - completely unused
2. REMOVE unused BlockType enum values: Stone, Grass, Sand (keep Air, Dirt only)
3. REMOVE unused Action enum values: LookUp, LookDown, LookLeft, LookRight, ToggleMenu  
4. DELETE VOX_LOG_* macros from logging.hpp (lines 56-59)
5. REMOVE glad_placeholder() function from render/glad/glad.c
6. DELETE unused InputManager methods: isActionJustPressed, isActionJustReleased, getMouseSensitivity, getCurrentContext, resetToDefaults, setKeyMapping, getKeyMapping, getActionName, setContext, setKeyMappingForContext, reloadConfig
7. REMOVE InputContext::Menu enum value and related unused context switching logic
8. DELETE World::unloadChunk() method (world.hpp:20, world.cpp:20)
9. CONSIDER removing Voxel::light property if no lighting system is planned

For each removal:
- Update CMakeLists.txt files to remove deleted source files
- Remove corresponding #include statements
- Update any configuration files (input.ini) that reference deleted actions
- Ensure no compilation errors after each step

Test the build after each major removal to ensure the application still functions correctly.
```

---

## Notes
- Total estimated lines of dead code: ~400-500 lines
- Removing dead code will improve compilation time and reduce binary size
- Some "dead" code may be intentionally added for future features - review each item before removal
- The input system has the most unused functionality, suggesting over-engineering for current requirements