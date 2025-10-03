## Voxel Engine 2025 (C++)

Lightweight voxel engine scaffold with:
- Voxel storage and world partitioning
- Mesh generation (visible faces; greedy meshing WIP)
- Minimal OpenGL demo with camera and editing
- Configurable logging with timestamps and rotating log files

## Build

WSL/Linux:
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DVOXEL_WITH_GL=ON ..
cmake --build . --config Release
```

Windows (MSVC):
```bat
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 -DVOXEL_WITH_GL=ON ..
cmake --build . --config Release
```

## Run (WSL + VcXsrv)

1) Start VcXsrv (Multiple windows, Start no client, Disable access control, Disable Native OpenGL)
2) In WSL:
```bash
export DISPLAY=:0.0
export LIBGL_ALWAYS_INDIRECT=1
./build/bin/voxel_app
```
If `:0.0` fails, use your Windows host IP for DISPLAY.

## Controls

- Mouse: look (inverted Y)
- WASD: free-fly relative to facing
- Left click: remove block (origin block protected)
- Right click: place block on highlighted face
- F: toggle wireframe
- R: recenter camera to world origin
- F3: toggle debug title (camera xyz, look vector, current hit voxel)
  - Title also shows an FPS counter (updates ~4x/second)

## Config

Edit `engine.ini` in project root:
```
chunk.size_x=16
chunk.size_y=16
chunk.size_z=16
log.level=info
log.file=
```
If `log.file` is empty, logs are written to `logs/engine_YYYYMMDD_HHMMSS.log` and the latest 50 files are kept.

Runtime logs
- Console logs stream live (flushed each line) and to rotating files in `logs/`
- Run errors (GLFW/X issues) are also written to `logs/run_errors.log` with session separators

## Troubleshooting (WSL + VcXsrv)

- Ensure VcXsrv is started with: Multiple windows, Start no client, Disable access control, Disable Native OpenGL, and allowed in Windows Firewall (Private+Public)
- In WSL, set display variables before running:
  ```bash
  export DISPLAY=127.0.0.1:0.0
  export LIBGL_ALWAYS_INDIRECT=1
  ./build/bin/voxel_app
  ```
- If windows do not appear: try software GL:
  ```bash
  export LIBGL_ALWAYS_SOFTWARE=1
  export MESA_LOADER_DRIVER_OVERRIDE=llvmpipe
  ```
- Check `logs/run_errors.log` for explicit messages (includes DISPLAY value)

## Notes

- Running on WSL requires an X server (VcXsrv) for the window.
- The demo uses immediate mode OpenGL for simplicity; a VBO/IBO + shader path can be added next.



