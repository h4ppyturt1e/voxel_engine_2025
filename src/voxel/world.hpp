#pragma once

#include <unordered_map>
#include <utility>
#include <cstddef>
#include "chunk.hpp"

namespace voxel {

struct ChunkCoordHash {
	std::size_t operator()(const std::pair<int,int>& v) const noexcept {
		return (static_cast<size_t>(v.first) << 32) ^ static_cast<size_t>(v.second);
	}
};

class World {
public:
	Chunk& getOrCreateChunk(int cx, int cz);
	bool hasChunk(int cx, int cz) const;
    bool unloadChunk(int cx, int cz);

private:
	std::unordered_map<std::pair<int,int>, Chunk, ChunkCoordHash> chunks_;
};

} // namespace voxel


