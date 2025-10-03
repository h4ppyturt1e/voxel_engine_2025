#pragma once

#include "../mesh/mesh.hpp"

namespace render {

class Renderer {
public:
	void initialize();
	void draw(const mesh::Mesh& mesh);
	void shutdown();
};

} // namespace render


