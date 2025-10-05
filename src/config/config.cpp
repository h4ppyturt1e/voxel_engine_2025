#include "config.hpp"

#include <fstream>
#include <sstream>

namespace config {

static void trim(std::string& s) {
	while (!s.empty() && (s.front()==' '||s.front()=='\t')) s.erase(s.begin());
	while (!s.empty() && (s.back()==' '||s.back()=='\t' || s.back()=='\r' || s.back()=='\n')) s.pop_back();
}

Config& Config::instance() {
	static Config cfg;
	return cfg;
}

bool Config::loadFromFile(const std::string& path) {
	std::ifstream in(path);
	if (!in) {
		return false;
	}
	std::string line;
	while (std::getline(in, line)) {
		trim(line);
		if (line.empty() || line[0]=='#' || line[0]==';') continue;
		size_t eq = line.find('=');
		if (eq == std::string::npos) continue;
		std::string key = line.substr(0, eq);
		std::string val = line.substr(eq+1);
		trim(key); trim(val);
		if (key == "chunk.size_x") chunk_.sizeX = std::stoi(val);
		else if (key == "chunk.size_y") chunk_.sizeY = std::stoi(val);
		else if (key == "chunk.size_z") chunk_.sizeZ = std::stoi(val);
		else if (key == "log.level") logging_.level = val;
		else if (key == "log.file") logging_.filePath = val;
		else if (key == "vsync") graphics_.vsync = (val == "true" || val == "1");
		else if (key == "graphics.resolution_width") graphics_.resolution_width = std::stoi(val);
		else if (key == "graphics.resolution_height") graphics_.resolution_height = std::stoi(val);
		else if (key == "graphics.quality") graphics_.quality = val;
		else if (key == "graphics.fullscreen") graphics_.fullscreen = (val == "true" || val == "1");
		else if (key == "ui.mouse_sensitivity") ui_.mouse_sensitivity = std::stof(val);
		else if (key == "ui.theme") ui_.theme = val;
		else if (key == "ui.scale") ui_.scale = std::stof(val);
		else if (key == "ui.crosshair_enabled") ui_.crosshair_enabled = (val == "true" || val == "1");
		else if (key == "ui.crosshair_percent") ui_.crosshair_percent = std::stof(val);
	else if (key == "build.time") build_time_ = val;
	}
	return true;
}

} // namespace config


