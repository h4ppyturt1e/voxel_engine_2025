#pragma once

#include <vector>
#include <cstdint>

namespace mesh {

struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
};

} // namespace mesh


