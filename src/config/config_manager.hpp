#pragma once

#include <string>

namespace config {

class ConfigManager {
public:
    static ConfigManager& instance();
    
    bool initialize();
    std::string getConfigPath(const std::string& configName) const;
    bool ensureConfigExists(const std::string& configName) const;
    
private:
    ConfigManager() = default;
    
    std::string runtimeConfigDir_;
    std::string defaultConfigDir_;
    
    bool copyDefaultConfig(const std::string& configName) const;
};

} // namespace config
