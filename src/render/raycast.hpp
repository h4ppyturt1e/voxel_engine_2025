#pragma once

namespace voxel { class Chunk; }

namespace render {

struct RayHit { 
    int x, y, z; 
    int nx, ny, nz; 
    bool hit; 
};

// Cast a ray through a voxel chunk and return the first hit
RayHit raycastVoxel(const voxel::Chunk& chunk, float ox, float oy, float oz, float dx, float dy, float dz, float maxDist);

} // namespace render
