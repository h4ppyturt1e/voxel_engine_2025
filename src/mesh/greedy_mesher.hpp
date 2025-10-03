#pragma once

#include "../voxel/chunk.hpp"
#include "mesh.hpp"

namespace mesh {

class GreedyMesher {
public:
	Mesh buildMesh(const voxel::Chunk& chunk);
};

} // namespace mesh


