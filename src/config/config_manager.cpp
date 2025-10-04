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
    // Set up paths - runtime config goes in same directory as executable
    // Get the executable's directory
    std::string exePath = std::filesystem::current_path().string();
    
    // If we're running from project root, the exe is in build/bin/Release/
    if (exePath.find("build") == std::string::npos) {
        exePath = "build/bin/Release";
    }
    
    runtimeConfigDir_ = exePath + "/config";
    defaultConfigDir_ = "default_config";
    
    // Create runtime config directory if it doesn't exist
    try {
        std::filesystem::create_directories(runtimeConfigDir_);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create config directory: " << e.what() << std::endl;
        return false;
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
            std::cerr << "Default config not found: " << defaultPath << std::endl;
            return false;
        }
        
        // Copy default to runtime location
        std::filesystem::copy_file(defaultPath, runtimePath);
        std::cout << "Copied default config: " << defaultPath << " -> " << runtimePath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to copy config: " << e.what() << std::endl;
        return false;
    }
}

} // namespace config
