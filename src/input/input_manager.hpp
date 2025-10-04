#pragma once

#include <string>
#include <unordered_map>

namespace input {


enum class InputContext {
    Game,
    Menu,
    Count
};

enum class Action {
    MoveForward, MoveBackward, MoveLeft, MoveRight, MoveUp, MoveDown,
    FastMovement,
    LookUp, LookDown, LookLeft, LookRight,
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
    bool isActionJustPressed(Action action) const;
    bool isActionJustReleased(Action action) const;
    
    void getMouseDelta(float& deltaX, float& deltaY) const;
    void setMouseSensitivity(float sensitivity);
    float getMouseSensitivity() const;
    
    void setKeyMapping(Action action, int key);
    int getKeyMapping(Action action) const;
    std::string getActionName(Action action) const;
    void resetToDefaults();
    
    // Context management
    void setContext(InputContext context);
    InputContext getCurrentContext() const;
    void setKeyMappingForContext(InputContext context, Action action, int key);
    int getKeyMappingForContext(InputContext context, Action action) const;
    
    // Hot reload functionality
    void reloadConfig();
    bool isConfigFileModified() const;
    
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