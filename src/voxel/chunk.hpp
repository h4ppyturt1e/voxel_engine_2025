#pragma once

#include <cstdint>
#include <vector>
#include "voxel.hpp"

namespace voxel {

class Chunk {
public:
    Chunk(int sizeX, int sizeY, int sizeZ);

    int sizeX() const { return sizeX_; }
    int sizeY() const { return sizeY_; }
    int sizeZ() const { return sizeZ_; }

    Voxel& at(int x, int y, int z);
    const Voxel& at(int x, int y, int z) const;

    bool saveToFile(const char* path) const;
    bool loadFromFile(const char* path);

private:
    int sizeX_;
    int sizeY_;
    int sizeZ_;
    std::vector<Voxel> voxels_;
    int index(int x, int y, int z) const {
        return (y * sizeZ_ + z) * sizeX_ + x;
    }
};

} // namespace voxel


