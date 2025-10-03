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

### Running the OpenGL demo on WSL with VcXsrv

If you are using WSL and want to open a windowed demo via GLFW:

1) Install and start VcXsrv on Windows
- Multiple windows
- Start no client
- Disable access control (checked)
- Disable Native OpenGL (checked)
- Allow through Windows Firewall (Private and Public)

2) Enable GL backend and build from WSL
```bash
cd /mnt/c/Users/<YOU>/Documents/GitHub/voxel_engine_2025
mkdir -p build && cd build
cmake -DVOXEL_WITH_GL=ON -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

3) Export display variables and run
```bash
export DISPLAY=:0.0
export LIBGL_ALWAYS_INDIRECT=1
./bin/voxel_app
```

If `:0.0` does not work, try:
```bash
export DISPLAY=$(grep -m1 nameserver /etc/resolv.conf | awk '{print $2}'):0.0
./bin/voxel_app
```

### Controls (GL demo)

- Mouse: look (inverted Y)
- WASD: free-fly movement relative to facing direction
- Q: remove block under crosshair (except the world origin block)
- E: place block in front of the hit (or a short distance ahead)
- R: recenter camera to look at world origin (0,0,0)
- F: toggle wireframe mode



