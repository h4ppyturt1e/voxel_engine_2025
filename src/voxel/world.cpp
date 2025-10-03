#include "world.hpp"

namespace voxel {

Chunk& World::getOrCreateChunk(int cx, int cz) {
	auto key = std::make_pair(cx, cz);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) {
		it = chunks_.emplace(key, Chunk{}).first;
	}
	return it->second;
}

bool World::hasChunk(int cx, int cz) const {
	return chunks_.find(std::make_pair(cx, cz)) != chunks_.end();
}

} // namespace voxel


