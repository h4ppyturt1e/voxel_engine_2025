#include "renderer.hpp"
#include "../core/logging.hpp"

namespace render {

void Renderer::initialize() {
	core::log(core::LogLevel::Info, "Renderer initialize (stub)");
}

void Renderer::draw(const mesh::Mesh&) {
	// Stub draw
}

void Renderer::shutdown() {
	core::log(core::LogLevel::Info, "Renderer shutdown (stub)");
}

void Renderer::setViewProj(const core::Mat4& view, const core::Mat4& proj) {
    view_ = view;
    proj_ = proj;
}

} // namespace render


