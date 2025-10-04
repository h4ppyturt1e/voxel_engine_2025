#include "raycast.hpp"
#include "../voxel/chunk.hpp"
#include "../voxel/voxel.hpp"
#include <cmath>

namespace render {

RayHit raycastVoxel(const voxel::Chunk& chunk, float ox, float oy, float oz, float dx, float dy, float dz, float maxDist) {
    RayHit r{0,0,0,0,0,0,false};
    const int sx = chunk.sizeX();
    const int sy = chunk.sizeY();
    const int sz = chunk.sizeZ();

    int x = (int)std::floor(ox);
    int y = (int)std::floor(oy);
    int z = (int)std::floor(oz);

    float t = 0.0f;
    int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
    int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);
    int stepZ = (dz > 0) ? 1 : (dz < 0 ? -1 : 0);

    float tDeltaX = (dx != 0.0f) ? std::abs(1.0f / dx) : 1e30f;
    float tDeltaY = (dy != 0.0f) ? std::abs(1.0f / dy) : 1e30f;
    float tDeltaZ = (dz != 0.0f) ? std::abs(1.0f / dz) : 1e30f;

    float nextVoxelBoundaryX = (stepX > 0) ? (std::floor(ox) + 1.0f) : std::floor(ox);
    float nextVoxelBoundaryY = (stepY > 0) ? (std::floor(oy) + 1.0f) : std::floor(oy);
    float nextVoxelBoundaryZ = (stepZ > 0) ? (std::floor(oz) + 1.0f) : std::floor(oz);

    float tMaxX = (dx != 0.0f) ? (nextVoxelBoundaryX - ox) / dx : 1e30f; if (tMaxX < 0) tMaxX = 0;
    float tMaxY = (dy != 0.0f) ? (nextVoxelBoundaryY - oy) / dy : 1e30f; if (tMaxY < 0) tMaxY = 0;
    float tMaxZ = (dz != 0.0f) ? (nextVoxelBoundaryZ - oz) / dz : 1e30f; if (tMaxZ < 0) tMaxZ = 0;

    while (t < maxDist) {
        if (x >= 0 && y >= 0 && z >= 0 && x < sx && y < sy && z < sz) {
            if (chunk.at(x, y, z).type != voxel::BlockType::Air) {
                r.hit = true;
                r.x = x; r.y = y; r.z = z;
                if (tMaxX < tMaxY && tMaxX < tMaxZ) {
                    r.nx = -stepX; r.ny = 0; r.nz = 0;
                } else if (tMaxY < tMaxZ) {
                    r.nx = 0; r.ny = -stepY; r.nz = 0;
                } else {
                    r.nx = 0; r.ny = 0; r.nz = -stepZ;
                }
                return r;
            }
        }
        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            t = tMaxX;
            tMaxX += tDeltaX;
            x += stepX;
        } else if (tMaxY < tMaxZ) {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
        } else {
            t = tMaxZ;
            tMaxZ += tDeltaZ;
            z += stepZ;
        }
    }
    return r;
}

} // namespace render
