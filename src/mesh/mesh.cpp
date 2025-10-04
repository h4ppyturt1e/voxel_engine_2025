#include "mesh.hpp"
#include "greedy_mesher.hpp"
#include "../voxel/chunk.hpp"
#include "../voxel/voxel.hpp"

namespace mesh {

static inline bool isSolid(const voxel::Voxel& v) {
	return v.type != voxel::BlockType::Air;
}

// Emit a single quad into the mesh (two triangles)
static void emitQuad(Mesh& out,
	float x0, float y0, float z0,
	float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3,
	float nx, float ny, float nz)
{
	std::uint32_t base = static_cast<std::uint32_t>(out.vertices.size());
	out.vertices.push_back(Vertex{ x0, y0, z0, nx, ny, nz, 0.0f, 0.0f });
	out.vertices.push_back(Vertex{ x1, y1, z1, nx, ny, nz, 1.0f, 0.0f });
	out.vertices.push_back(Vertex{ x2, y2, z2, nx, ny, nz, 1.0f, 1.0f });
	out.vertices.push_back(Vertex{ x3, y3, z3, nx, ny, nz, 0.0f, 1.0f });
	// Winding: counter-clockwise as seen from normal direction
	out.indices.push_back(base + 0);
	out.indices.push_back(base + 1);
	out.indices.push_back(base + 2);
	out.indices.push_back(base + 0);
	out.indices.push_back(base + 2);
	out.indices.push_back(base + 3);
}

Mesh GreedyMesher::buildMesh(const voxel::Chunk& chunk) {
    Mesh out;
    const int sx = chunk.sizeX();
    const int sy = chunk.sizeY();
    const int sz = chunk.sizeZ();

    auto solidAt = [&](int x, int y, int z) -> bool {
        if (x < 0 || y < 0 || z < 0 || x >= sx || y >= sy || z >= sz) return false; // outside is air
        return isSolid(chunk.at(x,y,z));
    };

    // For each voxel, emit faces where neighbor is air
    for (int z = 0; z < sz; ++z) {
        for (int y = 0; y < sy; ++y) {
            for (int x = 0; x < sx; ++x) {
                if (!solidAt(x,y,z)) continue;
                // +X
                if (!solidAt(x+1,y,z)) {
                    emitQuad(out,
                        x+1, y,   z,
                        x+1, y,   z+1,
                        x+1, y+1, z+1,
                        x+1, y+1, z,
                        1,0,0);
                }
                // -X
                if (!solidAt(x-1,y,z)) {
                    emitQuad(out,
                        x, y,   z,
                        x, y+1, z,
                        x, y+1, z+1,
                        x, y,   z+1,
                        -1,0,0);
                }
                // +Y
                if (!solidAt(x,y+1,z)) {
                    emitQuad(out,
                        x,   y+1, z,
                        x+1, y+1, z,
                        x+1, y+1, z+1,
                        x,   y+1, z+1,
                        0,1,0);
                }
                // -Y
                if (!solidAt(x,y-1,z)) {
                    emitQuad(out,
                        x,   y, z,
                        x,   y, z+1,
                        x+1, y, z+1,
                        x+1, y, z,
                        0,-1,0);
                }
                // +Z
                if (!solidAt(x,y,z+1)) {
                    emitQuad(out,
                        x,   y,   z+1,
                        x,   y+1, z+1,
                        x+1, y+1, z+1,
                        x+1, y,   z+1,
                        0,0,1);
                }
                // -Z
                if (!solidAt(x,y,z-1)) {
                    emitQuad(out,
                        x,   y,   z,
                        x+1, y,   z,
                        x+1, y+1, z,
                        x,   y+1, z,
                        0,0,-1);
                }
            }
        }
    }
    return out;
}

} // namespace mesh


