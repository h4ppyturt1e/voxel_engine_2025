#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>

#ifdef VOXEL_WITH_GL
struct GLFWwindow;
namespace ImGui { class Context; }
#else
struct GLFWwindow;
#endif

namespace ui {

// Forward declarations
class Overlay;

enum class OverlayType {
    HUD,
    Settings,
    KeyBindings
};

enum class GameState {
    Running,
    Paused
};

class UIManager {
public:
    static UIManager& instance();
    
    // Initialize/Shutdown
    bool initialize(void* glfwWindow = nullptr);
    void shutdown();
    
    // Frame lifecycle
    void beginFrame(float deltaTime = 0.016f);
    void endFrame();
    
    // Overlay management
    void showOverlay(OverlayType type);
    void hideOverlay(OverlayType type);
    void toggleOverlay(OverlayType type);
    bool isOverlayVisible(OverlayType type) const;
    
    // Modal behavior
    void setModal(OverlayType type, bool modal = true);
    bool isModalActive() const;
    bool shouldBlockInput() const;
    
    // Game state management
    GameState getGameState() const { return game_state_; }
    void setGameState(GameState state);
    bool isGamePaused() const { return game_state_ != GameState::Running; }
    
    // Settings management
    void loadSettings();
    void saveSettings();
    void applySettings();

    // Graphics runtime controls
    void setVSync(bool enabled);
    
    // Event system
    using OverlayEventCallback = std::function<void(OverlayType, bool)>;
    void setOverlayEventCallback(OverlayEventCallback callback);
    
private:
    UIManager() = default;
    ~UIManager() = default;
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    
    void initializeImGui(GLFWwindow* window);
    void shutdownImGui();
    
    std::vector<std::unique_ptr<Overlay>> overlays_;
    std::vector<bool> overlay_visibility_;
    std::vector<bool> overlay_modal_;
    OverlayType active_modal_ = OverlayType::HUD; // HUD is never modal
    
    GameState game_state_ = GameState::Running;
    
    OverlayEventCallback overlay_callback_;
    
    bool initialized_ = false;

#ifdef VOXEL_WITH_GL
    GLFWwindow* glfw_window_ = nullptr;
#endif
};

} // namespace ui
