#include "config_manager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace config {

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::initialize() {
    // Determine a robust base directory for runtime configs
    // Prefer current working directory; if no config folder there, fall back to build/bin/Release if present
    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path baseDir = cwd;
    if (!std::filesystem::exists(baseDir / "config")) {
        if (std::filesystem::exists(cwd / "build" / "bin" / "Release")) {
            baseDir = cwd / "build" / "bin" / "Release";
        }
    }
    runtimeConfigDir_ = (baseDir / "config").string();

    // Locate default_config directory by searching upwards from baseDir
    std::filesystem::path candidate;
    bool foundDefault = false;
    for (int up = 0; up <= 4; ++up) {
        std::filesystem::path probe = baseDir;
        for (int i = 0; i < up; ++i) probe = probe.parent_path();
        candidate = probe / "default_config";
        if (std::filesystem::exists(candidate) && std::filesystem::is_directory(candidate)) {
            foundDefault = true;
            break;
        }
    }
    defaultConfigDir_ = foundDefault ? candidate.string() : std::string("default_config");
    
    // Create runtime config directory if it doesn't exist
    try {
        std::filesystem::create_directories(runtimeConfigDir_);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create config directory: " << e.what() << std::endl;
        return false;
    }
    // Proactively copy any defaults that are missing (engine.ini, input.ini, theme.ini, etc.)
    try {
        if (std::filesystem::exists(defaultConfigDir_) && std::filesystem::is_directory(defaultConfigDir_)) {
            for (const auto& entry : std::filesystem::directory_iterator(defaultConfigDir_)) {
            if (!entry.is_regular_file()) continue;
            const auto& srcPath = entry.path();
            std::string fileName = srcPath.filename().string();
            std::string dstPath = runtimeConfigDir_ + "/" + fileName;
            if (!std::filesystem::exists(dstPath)) {
                std::filesystem::copy_file(srcPath, dstPath);
            }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: failed to sync default configs: " << e.what() << std::endl;
        // Not fatal
    }
    
    return true;
}

std::string ConfigManager::getConfigPath(const std::string& configName) const {
    return runtimeConfigDir_ + "/" + configName;
}

bool ConfigManager::ensureConfigExists(const std::string& configName) const {
    std::string runtimePath = getConfigPath(configName);
    std::string defaultPath = defaultConfigDir_ + "/" + configName;
    
    // If runtime config exists, use it
    if (std::filesystem::exists(runtimePath)) {
        return true;
    }
    
    // Otherwise, copy from default
    return copyDefaultConfig(configName);
}

bool ConfigManager::copyDefaultConfig(const std::string& configName) const {
    std::string defaultPath = defaultConfigDir_ + "/" + configName;
    std::string runtimePath = getConfigPath(configName);
    
    try {
        // Check if default config exists
        if (!std::filesystem::exists(defaultPath)) {
            std::string absDefaultPath = std::filesystem::absolute(defaultPath).string();
            std::cerr << "Default config not found: " << absDefaultPath << std::endl;
            return false;
        }
        
        // Copy default to runtime location
        std::filesystem::copy_file(defaultPath, runtimePath);
        std::string absDefaultPath = std::filesystem::absolute(defaultPath).string();
        std::string absRuntimePath = std::filesystem::absolute(runtimePath).string();
        std::cout << "Copied default config: " << absDefaultPath << " -> " << absRuntimePath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to copy config: " << e.what() << std::endl;
        return false;
    }
}

} // namespace config
