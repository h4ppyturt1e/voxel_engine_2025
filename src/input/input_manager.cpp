#include "input_manager.hpp"
#include "key_constants.hpp"
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <chrono>
#include <unordered_map>

namespace input {

InputManager& InputManager::instance() {
    static InputManager instance;
    return instance;
}

bool InputManager::initialize() {
    // No hardcoded mappings - everything comes from config file
    return true;
}

bool InputManager::loadConfig(const std::string& configPath) {
    
    configPath_ = configPath;
    
    // Initialize timestamp on first load
    if (lastConfigModTime_ == 0) {
        try {
            auto fileTime = std::filesystem::last_write_time(configPath);
            lastConfigModTime_ = std::chrono::duration_cast<std::chrono::seconds>(
                fileTime.time_since_epoch()).count();
        } catch (const std::exception&) {
            lastConfigModTime_ = 0;
        }
    }
    
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;
        
        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Parse action and key mapping
        Action action = stringToAction(key);
        if (action != Action::Count) {
            int keyCode = keyNameToCode(value);
            if (keyCode != -1) {
                actionToKey_[action] = keyCode;
                // Also update current context mapping
                contextMappings_[currentContext_][action] = keyCode;
            }
        } else if (key == "mouse_sensitivity") {
            try {
                mouseSensitivity_ = std::stof(value);
            } catch (const std::exception&) {
                // Keep default value
            }
        }
    }
    return true;
}

bool InputManager::saveConfig(const std::string& configPath) {
    std::ofstream file(configPath);
    if (!file.is_open()) return false;
    
    file << "# Input Configuration File\n";
    file << "# Format: action=key_name\n\n";
    
    file << "[actions]\n";
    for (const auto& pair : actionToKey_) {
        Action action = pair.first;
        int keyCode = pair.second;
        std::string keyName = keyCodeToName(keyCode);
        if (!keyName.empty()) {
            file << actionToString(action) << "=" << keyName << "\n";
        }
    }
    
    file << "\n[settings]\n";
    file << "mouse_sensitivity=" << mouseSensitivity_ << "\n";
    return true;
}

void InputManager::update() {
    // Check for config file changes and reload if needed
    if (isConfigFileModified()) {
        reloadConfig();
    }
    
    prevKeyStates_ = keyStates_;
    mouseDeltaX_ = 0.0f;
    mouseDeltaY_ = 0.0f;
}

bool InputManager::isActionPressed(Action action) const {
    // Check context-specific mapping first
    auto contextIt = contextMappings_.find(currentContext_);
    if (contextIt != contextMappings_.end()) {
        auto actionIt = contextIt->second.find(action);
        if (actionIt != contextIt->second.end()) {
            auto keyIt = keyStates_.find(actionIt->second);
            return keyIt != keyStates_.end() && keyIt->second;
        }
    }
    
    // Fall back to default mapping
    auto it = actionToKey_.find(action);
    if (it == actionToKey_.end()) return false;
    auto keyIt = keyStates_.find(it->second);
    return keyIt != keyStates_.end() && keyIt->second;
}

bool InputManager::isActionJustPressed(Action action) const {
    int key = getKeyMappingForContext(currentContext_, action);
    if (key == -1) {
        auto it = actionToKey_.find(action);
        if (it == actionToKey_.end()) return false;
        key = it->second;
    }
    bool current = keyStates_.find(key) != keyStates_.end() && keyStates_.at(key);
    bool previous = prevKeyStates_.find(key) != prevKeyStates_.end() && prevKeyStates_.at(key);
    return current && !previous;
}

bool InputManager::isActionJustReleased(Action action) const {
    int key = getKeyMappingForContext(currentContext_, action);
    if (key == -1) {
        auto it = actionToKey_.find(action);
        if (it == actionToKey_.end()) return false;
        key = it->second;
    }
    bool current = keyStates_.find(key) != keyStates_.end() && keyStates_.at(key);
    bool previous = prevKeyStates_.find(key) != prevKeyStates_.end() && prevKeyStates_.at(key);
    return !current && previous;
}

void InputManager::getMouseDelta(float& deltaX, float& deltaY) const {
    deltaX = mouseDeltaX_;
    deltaY = mouseDeltaY_;
}

void InputManager::setMouseSensitivity(float sensitivity) {
    mouseSensitivity_ = sensitivity;
}

float InputManager::getMouseSensitivity() const {
    return mouseSensitivity_;
}

void InputManager::setKeyMapping(Action action, int key) {
    actionToKey_[action] = key;
}

int InputManager::getKeyMapping(Action action) const {
    auto it = actionToKey_.find(action);
    return it != actionToKey_.end() ? it->second : -1;
}

std::string InputManager::getActionName(Action action) const {
    return actionToString(action);
}

void InputManager::resetToDefaults() {
    // Reset to defaults by reloading the config file
    if (!configPath_.empty()) {
        loadConfig(configPath_);
    }
}

void InputManager::setContext(InputContext context) {
    currentContext_ = context;
}

InputContext InputManager::getCurrentContext() const {
    return currentContext_;
}

void InputManager::setKeyMappingForContext(InputContext context, Action action, int key) {
    contextMappings_[context][action] = key;
}

int InputManager::getKeyMappingForContext(InputContext context, Action action) const {
    auto contextIt = contextMappings_.find(context);
    if (contextIt != contextMappings_.end()) {
        auto actionIt = contextIt->second.find(action);
        if (actionIt != contextIt->second.end()) {
            return actionIt->second;
        }
    }
    return -1;
}

void InputManager::setKeyState(int key, bool pressed) {
    keyStates_[key] = pressed;
}

void InputManager::setMouseDelta(float deltaX, float deltaY) {
    mouseDeltaX_ = deltaX * mouseSensitivity_;
    mouseDeltaY_ = deltaY * mouseSensitivity_;
}


void InputManager::setupContextMappings() {
    // Context mappings are now loaded from config file
    // This function is kept for future context-specific mappings
}

std::string InputManager::actionToString(Action action) const {
    switch (action) {
        case Action::MoveForward: return "MoveForward";
        case Action::MoveBackward: return "MoveBackward";
        case Action::MoveLeft: return "MoveLeft";
        case Action::MoveRight: return "MoveRight";
        case Action::MoveUp: return "MoveUp";
        case Action::MoveDown: return "MoveDown";
        case Action::FastMovement: return "FastMovement";
        case Action::LookUp: return "LookUp";
        case Action::LookDown: return "LookDown";
        case Action::LookLeft: return "LookLeft";
        case Action::LookRight: return "LookRight";
        case Action::ToggleMenu: return "ToggleMenu";
        case Action::ToggleDebug: return "ToggleDebug";
        case Action::ToggleWireframe: return "ToggleWireframe";
        case Action::ToggleMouseLock: return "ToggleMouseLock";
        case Action::ToggleVSync: return "ToggleVSync";
        case Action::RecenterCamera: return "RecenterCamera";
        case Action::BreakBlock: return "BreakBlock";
        case Action::PlaceBlock: return "PlaceBlock";
        default: return "Unknown";
    }
}

Action InputManager::stringToAction(const std::string& str) const {
    if (str == "MoveForward") return Action::MoveForward;
    if (str == "MoveBackward") return Action::MoveBackward;
    if (str == "MoveLeft") return Action::MoveLeft;
    if (str == "MoveRight") return Action::MoveRight;
    if (str == "MoveUp") return Action::MoveUp;
    if (str == "MoveDown") return Action::MoveDown;
    if (str == "FastMovement") return Action::FastMovement;
    if (str == "LookUp") return Action::LookUp;
    if (str == "LookDown") return Action::LookDown;
    if (str == "LookLeft") return Action::LookLeft;
    if (str == "LookRight") return Action::LookRight;
    if (str == "ToggleMenu") return Action::ToggleMenu;
    if (str == "ToggleDebug") return Action::ToggleDebug;
    if (str == "ToggleWireframe") return Action::ToggleWireframe;
    if (str == "ToggleMouseLock") return Action::ToggleMouseLock;
    if (str == "ToggleVSync") return Action::ToggleVSync;
    if (str == "RecenterCamera") return Action::RecenterCamera;
    if (str == "BreakBlock") return Action::BreakBlock;
    if (str == "PlaceBlock") return Action::PlaceBlock;
    return Action::Count;
}


void InputManager::reloadConfig() {
    if (!configPath_.empty()) {
        loadConfig(configPath_);
        
        // Update timestamp after successful reload
        try {
            auto fileTime = std::filesystem::last_write_time(configPath_);
            lastConfigModTime_ = std::chrono::duration_cast<std::chrono::seconds>(
                fileTime.time_since_epoch()).count();
        } catch (const std::exception&) {
            lastConfigModTime_ = 0;
        }
    }
}

bool InputManager::isConfigFileModified() const {
    if (configPath_.empty()) return false;
    
    try {
        auto fileTime = std::filesystem::last_write_time(configPath_);
        auto currentModTime = std::chrono::duration_cast<std::chrono::seconds>(
            fileTime.time_since_epoch()).count();
        return currentModTime > lastConfigModTime_;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace input