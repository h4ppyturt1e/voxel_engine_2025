#pragma once

#include <string>

namespace config {

struct ChunkDimensions {
	int sizeX {16};
	int sizeY {16};
	int sizeZ {16};
};

class Config {
public:
	static Config& instance();

	bool loadFromFile(const std::string& path);

	const ChunkDimensions& chunk() const { return chunk_; }

	struct Logging {
		std::string level {"info"};
		std::string filePath {};
	};

	const Logging& logging() const { return logging_; }

	struct Graphics {
		bool vsync {true};
		int resolution_width {800};
		int resolution_height {600};
		std::string quality {"medium"};
	};
	
	const Graphics& graphics() const { return graphics_; }
	Graphics& graphics() { return graphics_; }

	struct UI {
		float mouse_sensitivity {0.01f};
		std::string theme {"dark"};
		float scale {1.0f};
	};

	const UI& ui() const { return ui_; }
	UI& ui() { return ui_; }

    // Build info
    const std::string& buildTime() const { return build_time_; }
    void setBuildTime(const std::string& t) { build_time_ = t; }

private:
	ChunkDimensions chunk_{};
	Logging logging_{};
	Graphics graphics_{};
	UI ui_{};
    std::string build_time_{};
};

} // namespace config


