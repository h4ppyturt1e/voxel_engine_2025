#include "world.hpp"
#include "../config/config.hpp"

namespace voxel {

Chunk& World::getOrCreateChunk(int cx, int cz) {
	auto key = std::make_pair(cx, cz);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) {
		const auto& dims = config::Config::instance().chunk();
		it = chunks_.emplace(key, Chunk{dims.sizeX, dims.sizeY, dims.sizeZ}).first;
	}
	return it->second;
}

bool World::hasChunk(int cx, int cz) const {
	return chunks_.find(std::make_pair(cx, cz)) != chunks_.end();
}

bool World::unloadChunk(int cx, int cz) {
	return chunks_.erase(std::make_pair(cx, cz)) > 0;
}

} // namespace voxel


