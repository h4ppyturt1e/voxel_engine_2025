#include "input_manager.hpp"
#include "key_constants.hpp"
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <chrono>
#include <unordered_map>
#include "../core/logging.hpp"
#include "../config/config.hpp"

namespace input {

InputManager& InputManager::instance() {
    static InputManager instance;
    return instance;
}

bool InputManager::initialize() {
    // Load mouse sensitivity from the main config system
    mouseSensitivity_ = config::Config::instance().ui().mouse_sensitivity;
    core::log(core::LogLevel::Info, "InputManager initialized with mouse sensitivity: " + std::to_string(mouseSensitivity_));
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
        } 
        // Note: mouse_sensitivity is now loaded from main config system, not input.ini
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
    
    // Note: mouse_sensitivity is now managed by main config system, not saved here
    return true;
}

void InputManager::update() {
    prevKeyStates_ = keyStates_;
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

void InputManager::getMouseDelta(float& deltaX, float& deltaY) {
    deltaX = mouseDeltaX_;
    deltaY = mouseDeltaY_;
    // Reset mouse delta after consuming it
    mouseDeltaX_ = 0.0f;
    mouseDeltaY_ = 0.0f;
}

void InputManager::setMouseSensitivity(float sensitivity) {
    mouseSensitivity_ = sensitivity;
    core::log(core::LogLevel::Info, "Mouse sensitivity updated to: " + std::to_string(sensitivity));
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


} // namespace input