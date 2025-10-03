#pragma once

#include "../mesh/mesh.hpp"
#include "../core/math.hpp"

namespace render {

class Renderer {
public:
	void initialize();
	void draw(const mesh::Mesh& mesh);
	void shutdown();

    // Camera state
    void setViewProj(const core::Mat4& view, const core::Mat4& proj);
private:
    core::Mat4 view_{};
    core::Mat4 proj_{};
};

} // namespace render


