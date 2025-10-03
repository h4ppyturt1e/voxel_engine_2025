## Voxel Engine 2025 (C++)

A minimal C++ project scaffold for a voxel game engine. Modules:

- Voxel storage and world partitioning
- Mesh generation
- Rendering pipeline (stubbed)
- Camera controls
- Input handling

### Build

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

On Windows with Visual Studio generators, use the `--config` flag for Debug/Release.


