#pragma once

#include <array>
#include <cstdint>
#include "voxel.hpp"

namespace voxel {

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

class Chunk {
public:
	Chunk();

	Voxel& at(int x, int y, int z);
	const Voxel& at(int x, int y, int z) const;

private:
	std::array<Voxel, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> voxels_{};
	static constexpr int index(int x, int y, int z) {
		return (y * CHUNK_SIZE_Z + z) * CHUNK_SIZE_X + x;
	}
};

} // namespace voxel


