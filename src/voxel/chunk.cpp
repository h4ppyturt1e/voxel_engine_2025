#include "chunk.hpp"
#include <fstream>
#include <cstring>

namespace voxel {

Chunk::Chunk(int sizeX, int sizeY, int sizeZ)
	: sizeX_(sizeX), sizeY_(sizeY), sizeZ_(sizeZ), voxels_(static_cast<size_t>(sizeX) * sizeY * sizeZ) {}

Voxel& Chunk::at(int x, int y, int z) {
	return voxels_[index(x, y, z)];
}

const Voxel& Chunk::at(int x, int y, int z) const {
	return voxels_[index(x, y, z)];
}

static constexpr std::uint32_t kChunkMagic = 0x5643584C; // 'VCXL'

bool Chunk::saveToFile(const char* path) const {
	std::ofstream out(path, std::ios::binary);
	if (!out) return false;
	std::uint32_t magic = kChunkMagic;
	out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
	out.write(reinterpret_cast<const char*>(&sizeX_), sizeof(sizeX_));
	out.write(reinterpret_cast<const char*>(&sizeY_), sizeof(sizeY_));
	out.write(reinterpret_cast<const char*>(&sizeZ_), sizeof(sizeZ_));
	for (const Voxel& v : voxels_) {
		std::uint8_t type = static_cast<std::uint8_t>(v.type);
		out.write(reinterpret_cast<const char*>(&type), sizeof(type));
	}
	return true;
}

bool Chunk::loadFromFile(const char* path) {
	std::ifstream in(path, std::ios::binary);
	if (!in) return false;
	std::uint32_t magic = 0;
	in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
	if (magic != kChunkMagic) return false;
	int x=0,y=0,z=0;
	in.read(reinterpret_cast<char*>(&x), sizeof(x));
	in.read(reinterpret_cast<char*>(&y), sizeof(y));
	in.read(reinterpret_cast<char*>(&z), sizeof(z));
	if (x<=0||y<=0||z<=0) return false;
	sizeX_ = x; sizeY_ = y; sizeZ_ = z;
	voxels_.assign(static_cast<size_t>(sizeX_) * sizeY_ * sizeZ_, Voxel{});
	for (Voxel& v : voxels_) {
		std::uint8_t type = 0;
		in.read(reinterpret_cast<char*>(&type), sizeof(type));
		v.type = static_cast<BlockType>(type);
	}
	return true;
}

} // namespace voxel


