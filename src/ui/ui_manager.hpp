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
    void setWindowSize(int width, int height);
    void setFullscreen(bool enabled);
    void setFontOverride(const std::string& path, float size) {
        override_font_path_ = path;
        override_font_size_ = size;
    }
    const std::string& getFontOverridePath() const { return override_font_path_; }
    float getFontOverrideSize() const { return override_font_size_; }

    // Cursor lock controls
    void setCursorLocked(bool locked);
    bool isCursorLocked() const {
        return cursor_locked_;
    }
    
    // Event system
    using OverlayEventCallback = std::function<void(OverlayType, bool)>;
    void setOverlayEventCallback(OverlayEventCallback callback);
    // Mark font atlas dirty to rebuild GL texture safely next frame
    void markFontAtlasDirty() { font_atlas_dirty_ = true; }
    // Request theme (including fonts) to be re-applied at the start of next frame
    void markThemeDirty() { theme_dirty_ = true; }
    
private:
    UIManager() = default;
    ~UIManager() = default;
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    
    void initializeImGui(GLFWwindow* window);
    void shutdownImGui();
    void applyCursorMode();
    
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

    bool cursor_locked_ = false;

    // Optional font override provided by UI (session-scoped)
    std::string override_font_path_{};
    float override_font_size_ {16.0f};

    bool font_atlas_dirty_ = false;
    bool theme_dirty_ = false;
};

} // namespace ui
