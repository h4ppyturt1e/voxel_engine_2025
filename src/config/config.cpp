#include "config.hpp"

#include <fstream>
#include <sstream>
#include "ini_parser.hpp"

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
    IniParser parser;
    if (!parser.parseFile(path)) return false;
    for (const auto& [key, val] : parser.entries()) {
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


