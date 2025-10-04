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
	};
	
	const Graphics& graphics() const { return graphics_; }

private:
	ChunkDimensions chunk_{};
	Logging logging_{};
	Graphics graphics_{};
};

} // namespace config


