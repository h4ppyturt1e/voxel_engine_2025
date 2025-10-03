#pragma once

#include <cstdint>

namespace voxel {

struct Voxel {
	std::uint8_t id {0};
	std::uint8_t light {0};
};

} // namespace voxel


