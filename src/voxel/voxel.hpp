#pragma once

#include <cstdint>

namespace voxel {

enum class BlockType : std::uint8_t {
	Air = 0,
	Dirt = 1,
	Stone = 2,
	Grass = 3,
	Sand = 4
};

struct Voxel {
	BlockType type { BlockType::Air };
	std::uint8_t light {0};
};

} // namespace voxel


