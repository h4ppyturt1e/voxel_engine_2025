#include <iostream>

#include "../core/logging.hpp"
#include "../config/config.hpp"
#include "../voxel/world.hpp"
#include "../voxel/world_manager.hpp"
#include "../mesh/greedy_mesher.hpp"
#include "../render/gl_app.hpp"

int main() {
    // Configure logging from config (try cwd and parent)
    bool cfgOk = config::Config::instance().loadFromFile("engine.ini");
    if (!cfgOk) {
        cfgOk = config::Config::instance().loadFromFile("../engine.ini");
    }
    const auto& logcfg = config::Config::instance().logging();
    if (logcfg.level == "debug") core::setLogLevel(core::LogLevel::Debug);
    else if (logcfg.level == "info") core::setLogLevel(core::LogLevel::Info);
    else if (logcfg.level == "warn") core::setLogLevel(core::LogLevel::Warn);
    else if (logcfg.level == "error") core::setLogLevel(core::LogLevel::Error);
    if (!logcfg.filePath.empty()) {
        core::Logger::instance().addSink(std::make_shared<core::FileSink>(logcfg.filePath));
    }

    core::log(core::LogLevel::Info, "Voxel Engine 2025 starting up...");
    core::log(core::LogLevel::Info, "Initializing subsystems (voxel, mesh, render, camera, input)...");

	if (!cfgOk) {
		core::log(core::LogLevel::Warn, "Failed to load engine.ini, using defaults.");
	}
	const auto& dims = config::Config::instance().chunk();
	core::log(core::LogLevel::Info, "Chunk dims: " + std::to_string(dims.sizeX) + "x" + std::to_string(dims.sizeY) + "x" + std::to_string(dims.sizeZ));

	// Smoke test chunk create + serialize
    voxel::World world;
    voxel::WorldManager wm(world);
    wm.setViewDistance(2);
    wm.updatePlayerPosition(0.0f, 0.0f, 0.0f);
    voxel::Chunk& c = world.getOrCreateChunk(0, 0);
    voxel::Voxel v; v.type = voxel::BlockType::Dirt;
    for (int z = 0; z < 4; ++z) {
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                c.at(x, y, z) = v;
            }
        }
    }
    c.saveToFile("chunk_0_0.vxl");
    core::log(core::LogLevel::Info, "Saved chunk to chunk_0_0.vxl");

    // Build mesh for this chunk
    mesh::GreedyMesher gm;
    mesh::Mesh m = gm.buildMesh(c);
    core::log(core::LogLevel::Info, "Mesh: vertices=" + std::to_string(m.vertices.size()) + ", indices=" + std::to_string(m.indices.size()));

#ifdef VOXEL_WITH_GL
    core::log(core::LogLevel::Info, "GL demo: enabled (opening window)...");
    render::run_demo(m);
#else
    core::log(core::LogLevel::Info, "GL demo: disabled (VOXEL_WITH_GL=OFF)");
#endif

	core::log(core::LogLevel::Info, "Shutdown.");
	return 0;
}


