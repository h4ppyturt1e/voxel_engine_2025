#pragma once

#include "../mesh/mesh.hpp"
namespace voxel { class World; }
namespace mesh { class GreedyMesher; }

namespace render {

#ifdef VOXEL_WITH_GL
// Run a minimal GL demo window to render and edit voxels in the provided world
int run_demo(voxel::World& world, mesh::GreedyMesher& mesher);
#endif

} // namespace render


