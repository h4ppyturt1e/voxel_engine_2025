#include "world_manager.hpp"
#include "../config/config.hpp"

namespace voxel {

WorldManager::WorldManager(World& world) : world_(world) {
	const auto& dims = config::Config::instance().chunk();
	chunkSizeX_ = dims.sizeX;
	chunkSizeY_ = dims.sizeY;
	chunkSizeZ_ = dims.sizeZ;
}

void WorldManager::setViewDistance(int chunksRadius) { viewDistance_ = chunksRadius; }

int WorldManager::floorDiv(int a, int b) {
	int q = a / b;
	int r = a % b;
	if ((r != 0) && ((r < 0) != (b < 0))) --q;
	return q;
}

int WorldManager::mod(int a, int b) {
	int m = a % b;
	if (m < 0) m += (b < 0 ? -b : b);
	return m;
}

void WorldManager::ensureChunksAround(int cx, int cz) {
	for (int dz = -viewDistance_; dz <= viewDistance_; ++dz) {
		for (int dx = -viewDistance_; dx <= viewDistance_; ++dx) {
			world_.getOrCreateChunk(cx + dx, cz + dz);
		}
	}
	// Optional: unload beyond radius - simple pass for now
}

void WorldManager::updatePlayerPosition(float x, float, float z) {
	int cx = floorDiv(static_cast<int>(x), chunkSizeX_);
	int cz = floorDiv(static_cast<int>(z), chunkSizeZ_);
	if (cx != playerChunkX_ || cz != playerChunkZ_) {
		playerChunkX_ = cx; playerChunkZ_ = cz;
		ensureChunksAround(cx, cz);
	}
}

Voxel* WorldManager::tryGetVoxel(int x, int y, int z) {
	int cx = floorDiv(x, chunkSizeX_);
	int cz = floorDiv(z, chunkSizeZ_);
	if (!world_.hasChunk(cx, cz)) return nullptr;
	int lx = mod(x, chunkSizeX_);
	int ly = y;
	int lz = mod(z, chunkSizeZ_);
	if (ly < 0 || ly >= chunkSizeY_) return nullptr;
	Chunk& c = world_.getOrCreateChunk(cx, cz);
	return &c.at(lx, ly, lz);
}

bool WorldManager::setVoxel(int x, int y, int z, const Voxel& v) {
	Voxel* p = tryGetVoxel(x, y, z);
	if (!p) return false;
	*p = v;
	return true;
}

} // namespace voxel


