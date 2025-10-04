#include "input_manager.hpp"
#include "key_constants.hpp"
#include <fstream>

namespace input {

InputManager& InputManager::instance() {
    static InputManager instance;
    return instance;
}

bool InputManager::initialize() {
    setupDefaultMappings();
    return true;
}

bool InputManager::loadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) return false;
    
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
    prevKeyStates_ = keyStates_;
    mouseDeltaX_ = 0.0f;
    mouseDeltaY_ = 0.0f;
}

bool InputManager::isActionPressed(Action action) const {
    auto it = actionToKey_.find(action);
    if (it == actionToKey_.end()) return false;
    auto keyIt = keyStates_.find(it->second);
    return keyIt != keyStates_.end() && keyIt->second;
}

bool InputManager::isActionJustPressed(Action action) const {
    auto it = actionToKey_.find(action);
    if (it == actionToKey_.end()) return false;
    int key = it->second;
    bool current = keyStates_.find(key) != keyStates_.end() && keyStates_.at(key);
    bool previous = prevKeyStates_.find(key) != prevKeyStates_.end() && prevKeyStates_.at(key);
    return current && !previous;
}

bool InputManager::isActionJustReleased(Action action) const {
    auto it = actionToKey_.find(action);
    if (it == actionToKey_.end()) return false;
    int key = it->second;
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
    setupDefaultMappings();
}

void InputManager::setKeyState(int key, bool pressed) {
    keyStates_[key] = pressed;
}

void InputManager::setMouseDelta(float deltaX, float deltaY) {
    mouseDeltaX_ = deltaX * mouseSensitivity_;
    mouseDeltaY_ = deltaY * mouseSensitivity_;
}

void InputManager::setupDefaultMappings() {
    actionToKey_[Action::MoveForward] = KEY_W;
    actionToKey_[Action::MoveBackward] = KEY_S;
    actionToKey_[Action::MoveLeft] = KEY_A;
    actionToKey_[Action::MoveRight] = KEY_D;
    actionToKey_[Action::MoveUp] = KEY_SPACE;
    actionToKey_[Action::MoveDown] = KEY_LEFT_CONTROL;
    actionToKey_[Action::FastMovement] = KEY_LEFT_SHIFT;
    actionToKey_[Action::LookUp] = KEY_UP;
    actionToKey_[Action::LookDown] = KEY_DOWN;
    actionToKey_[Action::LookLeft] = KEY_LEFT;
    actionToKey_[Action::LookRight] = KEY_RIGHT;
    actionToKey_[Action::ToggleMenu] = KEY_ESCAPE;
    actionToKey_[Action::ToggleDebug] = KEY_F3;
    actionToKey_[Action::ToggleWireframe] = KEY_F;
    actionToKey_[Action::ToggleMouseLock] = KEY_F4;
    actionToKey_[Action::ToggleVSync] = KEY_F5;
    actionToKey_[Action::RecenterCamera] = KEY_R;
    actionToKey_[Action::BreakBlock] = MOUSE_BUTTON_LEFT;
    actionToKey_[Action::PlaceBlock] = MOUSE_BUTTON_RIGHT;
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

int InputManager::keyNameToCode(const std::string& keyName) const {
    if (keyName == "W") return KEY_W;
    if (keyName == "A") return KEY_A;
    if (keyName == "S") return KEY_S;
    if (keyName == "D") return KEY_D;
    if (keyName == "SPACE") return KEY_SPACE;
    if (keyName == "SHIFT") return KEY_LEFT_SHIFT;
    if (keyName == "CTRL") return KEY_LEFT_CONTROL;
    if (keyName == "UP") return KEY_UP;
    if (keyName == "DOWN") return KEY_DOWN;
    if (keyName == "LEFT") return KEY_LEFT;
    if (keyName == "RIGHT") return KEY_RIGHT;
    if (keyName == "ESCAPE") return KEY_ESCAPE;
    if (keyName == "F") return KEY_F;
    if (keyName == "R") return KEY_R;
    if (keyName == "F3") return KEY_F3;
    if (keyName == "F4") return KEY_F4;
    if (keyName == "F5") return KEY_F5;
    if (keyName == "MOUSE_LEFT") return MOUSE_BUTTON_LEFT;
    if (keyName == "MOUSE_RIGHT") return MOUSE_BUTTON_RIGHT;
    return -1;
}

std::string InputManager::keyCodeToName(int keyCode) const {
    switch (keyCode) {
        case KEY_W: return "W";
        case KEY_A: return "A";
        case KEY_S: return "S";
        case KEY_D: return "D";
        case KEY_SPACE: return "SPACE";
        case KEY_LEFT_SHIFT: return "SHIFT";
        case KEY_LEFT_CONTROL: return "CTRL";
        case KEY_UP: return "UP";
        case KEY_DOWN: return "DOWN";
        case KEY_LEFT: return "LEFT";
        case KEY_RIGHT: return "RIGHT";
        case KEY_ESCAPE: return "ESCAPE";
        case KEY_F: return "F";
        case KEY_R: return "R";
        case KEY_F3: return "F3";
        case KEY_F4: return "F4";
        case KEY_F5: return "F5";
        case MOUSE_BUTTON_LEFT: return "MOUSE_LEFT";
        case MOUSE_BUTTON_RIGHT: return "MOUSE_RIGHT";
        default: return "";
    }
}

} // namespace input