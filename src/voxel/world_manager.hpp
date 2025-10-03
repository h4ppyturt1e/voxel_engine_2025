#pragma once

#include "world.hpp"

namespace voxel {

class WorldManager {
public:
	explicit WorldManager(World& world);

	void setViewDistance(int chunksRadius);
	void updatePlayerPosition(float x, float y, float z);

	// Global coordinate voxel access (x,y,z in world space)
	Voxel* tryGetVoxel(int x, int y, int z);
	bool setVoxel(int x, int y, int z, const Voxel& v);

private:
	World& world_;
	int viewDistance_ { 4 };
	int chunkSizeX_ { 16 };
	int chunkSizeY_ { 16 };
	int chunkSizeZ_ { 16 };
	int playerChunkX_ { 0 };
	int playerChunkZ_ { 0 };

	void ensureChunksAround(int cx, int cz);
	static int floorDiv(int a, int b);
	static int mod(int a, int b);
};

} // namespace voxel


