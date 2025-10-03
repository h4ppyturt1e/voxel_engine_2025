#include <iostream>

#include "../core/logging.hpp"

int main() {
	core::setLogLevel(core::LogLevel::Info);
	core::log(core::LogLevel::Info, "Voxel Engine 2025 starting up...");
	core::log(core::LogLevel::Info, "Initializing subsystems (voxel, mesh, render, camera, input)...");

	core::log(core::LogLevel::Info, "Shutdown.");
	return 0;
}


