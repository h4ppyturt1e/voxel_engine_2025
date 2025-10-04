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
    // Debug: printf("loadConfig called with path: %s\n", configPath.c_str());
    
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
        // Debug: printf("Failed to open config file: %s\n", configPath.c_str());
        return false;
    }
    // Debug: printf("Successfully opened config file: %s\n", configPath.c_str());
    
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
                // Debug: printf("Loaded mapping: %s -> %s (keyCode: %d)\n", key.c_str(), value.c_str(), keyCode);
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
        // Add debug logging
        printf("Input config reloaded!\n");
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
            if (action == Action::MoveForward) {
                // Debug: printf("Checking MoveForward: context key=%d, pressed=%d\n", actionIt->second, keyIt != keyStates_.end() && keyIt->second);
            }
            return keyIt != keyStates_.end() && keyIt->second;
        }
    }
    
    // Fall back to default mapping
    auto it = actionToKey_.find(action);
    if (it == actionToKey_.end()) return false;
    auto keyIt = keyStates_.find(it->second);
    if (action == Action::MoveForward) {
        printf("Checking MoveForward: default key=%d, pressed=%d\n", it->second, keyIt != keyStates_.end() && keyIt->second);
    }
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

int keyNameToCode(const std::string& keyName) {
    static const std::unordered_map<std::string, int> keyMap = {
        // Letters
        {"A", 65}, {"B", 66}, {"C", 67}, {"D", 68}, {"E", 69}, {"F", 70},
        {"G", 71}, {"H", 72}, {"I", 73}, {"J", 74}, {"K", 75}, {"L", 76},
        {"M", 77}, {"N", 78}, {"O", 79}, {"P", 80}, {"Q", 81}, {"R", 82},
        {"S", 83}, {"T", 84}, {"U", 85}, {"V", 86}, {"W", 87}, {"X", 88},
        {"Y", 89}, {"Z", 90},
        
        // Numbers
        {"0", 48}, {"1", 49}, {"2", 50}, {"3", 51}, {"4", 52},
        {"5", 53}, {"6", 54}, {"7", 55}, {"8", 56}, {"9", 57},
        
        // Special keys
        {"SPACE", 32}, {"ENTER", 257}, {"ESCAPE", 256},
        {"SHIFT", 340}, {"CTRL", 341}, {"ALT", 342},
        
        // Arrow keys
        {"UP", 265}, {"DOWN", 264}, {"LEFT", 263}, {"RIGHT", 262},
        
        // Function keys
        {"F1", 290}, {"F2", 291}, {"F3", 292}, {"F4", 293}, {"F5", 294},
        {"F6", 295}, {"F7", 296}, {"F8", 297}, {"F9", 298}, {"F10", 299},
        {"F11", 300}, {"F12", 301},
        
        // Mouse buttons
        {"MOUSE_LEFT", 0}, {"MOUSE_RIGHT", 1}, {"MOUSE_MIDDLE", 2}
    };
    
    auto it = keyMap.find(keyName);
    return (it != keyMap.end()) ? it->second : -1;
}

std::string keyCodeToName(int keyCode) {
    static const std::unordered_map<int, std::string> codeMap = {
        // Letters
        {65, "A"}, {66, "B"}, {67, "C"}, {68, "D"}, {69, "E"}, {70, "F"},
        {71, "G"}, {72, "H"}, {73, "I"}, {74, "J"}, {75, "K"}, {76, "L"},
        {77, "M"}, {78, "N"}, {79, "O"}, {80, "P"}, {81, "Q"}, {82, "R"},
        {83, "S"}, {84, "T"}, {85, "U"}, {86, "V"}, {87, "W"}, {88, "X"},
        {89, "Y"}, {90, "Z"},
        
        // Numbers
        {48, "0"}, {49, "1"}, {50, "2"}, {51, "3"}, {52, "4"},
        {53, "5"}, {54, "6"}, {55, "7"}, {56, "8"}, {57, "9"},
        
        // Special keys
        {32, "SPACE"}, {257, "ENTER"}, {256, "ESCAPE"},
        {340, "SHIFT"}, {341, "CTRL"}, {342, "ALT"},
        
        // Arrow keys
        {265, "UP"}, {264, "DOWN"}, {263, "LEFT"}, {262, "RIGHT"},
        
        // Function keys
        {290, "F1"}, {291, "F2"}, {292, "F3"}, {293, "F4"}, {294, "F5"},
        {295, "F6"}, {296, "F7"}, {297, "F8"}, {298, "F9"}, {299, "F10"},
        {300, "F11"}, {301, "F12"},
        
        // Mouse buttons
        {0, "MOUSE_LEFT"}, {1, "MOUSE_RIGHT"}, {2, "MOUSE_MIDDLE"}
    };
    
    auto it = codeMap.find(keyCode);
    return (it != codeMap.end()) ? it->second : "";
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