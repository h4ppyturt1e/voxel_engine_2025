#include "input_manager.hpp"
#include "key_constants.hpp"
#include "../config/ini_parser.hpp"
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
    
    config::IniParser parser;
    if (!parser.parseFile(configPath)) return false;
    // Read [actions] section
    auto actions = parser.section("actions");
    for (const auto& [actionName, keyName] : actions) {
        Action action = stringToAction(actionName);
        if (action != Action::Count) {
            int keyCode = keyNameToCode(keyName);
            if (keyCode != -1) {
                actionToKey_[action] = keyCode;
                contextMappings_[currentContext_][action] = keyCode;
            }
        }
    }
    return true;
}

std::unordered_map<Action, int> InputManager::getInputBindings() {
    return actionToKey_;
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


// registers a callback that will be called on the next key press
// this callback will be used on setKeyState
void InputManager::waitForNextKey(std::function<bool(int)> callback) {
    if (waitingForKeyCallback_) {
        core::log(core::LogLevel::Warn, "Already waiting for a key press. Ignoring new callback.");
        return;
    }

    waitingForKeyCallback_ = callback;
    }

void InputManager::setKeyState(int key, bool pressed) {
    if (waitingForKeyCallback_ && pressed) {
        bool result = waitingForKeyCallback_(key);
        if (result) waitingForKeyCallback_ = nullptr;
    }
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