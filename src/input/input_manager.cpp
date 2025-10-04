#include "input_manager.hpp"
#include "key_constants.hpp"

namespace input {

InputManager& InputManager::instance() {
    static InputManager instance;
    return instance;
}

bool InputManager::initialize() {
    setupDefaultMappings();
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
    actionToKey_[Action::MoveDown] = KEY_LEFT_SHIFT;
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

} // namespace input