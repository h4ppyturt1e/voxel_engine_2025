#pragma once

#include "../mesh/mesh.hpp"

namespace render {

#ifdef VOXEL_WITH_GL
// Run a minimal GL demo window to render the provided mesh
int run_demo(const mesh::Mesh& mesh);
#endif

} // namespace render


