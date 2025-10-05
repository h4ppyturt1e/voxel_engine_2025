#pragma once

#include <cstdint>

namespace voxel {

enum class BlockType : std::uint8_t {
	Air = 0,
	Dirt = 1
};

struct Voxel {
	BlockType type { BlockType::Air };
};

} // namespace voxel


