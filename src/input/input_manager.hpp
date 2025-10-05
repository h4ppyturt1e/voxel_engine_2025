#pragma once

#include <string>
#include <unordered_map>

namespace input {


enum class InputContext {
    Game,
    Count
};

enum class Action {
    MoveForward, MoveBackward, MoveLeft, MoveRight, MoveUp, MoveDown,
    FastMovement,
    ToggleMenu, ToggleDebug, ToggleWireframe, ToggleMouseLock, ToggleVSync, RecenterCamera,
    BreakBlock, PlaceBlock,
    Count
};

class InputManager {
public:
    static InputManager& instance();
    
    bool initialize();
    bool loadConfig(const std::string& configPath);
    bool saveConfig(const std::string& configPath);
    void update();
    
    bool isActionPressed(Action action) const;
    void getMouseDelta(float& deltaX, float& deltaY);
    
    void setKeyState(int key, bool pressed);
    void setMouseDelta(float deltaX, float deltaY);

private:
    InputManager() = default;
    
    std::unordered_map<Action, int> actionToKey_;
    std::unordered_map<InputContext, std::unordered_map<Action, int>> contextMappings_;
    std::unordered_map<int, bool> keyStates_;
    std::unordered_map<int, bool> prevKeyStates_;
    
    InputContext currentContext_{InputContext::Game};
    
    float mouseDeltaX_{0.0f};
    float mouseDeltaY_{0.0f};
    float mouseSensitivity_{0.01f};
    
    // Hot reload state
    std::string configPath_;
    std::int64_t lastConfigModTime_{0};
    
    void setupContextMappings();
    std::string actionToString(Action action) const;
    Action stringToAction(const std::string& str) const;
};

} // namespace input