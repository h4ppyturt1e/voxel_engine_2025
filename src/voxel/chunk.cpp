#include "chunk.hpp"

namespace voxel {

Chunk::Chunk() = default;

Voxel& Chunk::at(int x, int y, int z) {
	return voxels_[index(x, y, z)];
}

const Voxel& Chunk::at(int x, int y, int z) const {
	return voxels_[index(x, y, z)];
}

} // namespace voxel


