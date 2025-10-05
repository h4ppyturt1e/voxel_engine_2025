#include "ui_manager.hpp"
#include "overlay.hpp"
#include "settings_menu.hpp"
#include "key_bindings_menu.hpp"
#include "hud.hpp"

#include "../config/config.hpp"
#include "../config/config_manager.hpp"
#include "../core/logging.hpp"

#ifdef VOXEL_WITH_GL
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/gl.h>
#endif

#include <algorithm>

namespace ui {

UIManager& UIManager::instance() {
    static UIManager instance;
    return instance;
}

bool UIManager::initialize(void* glfwWindow) {
    if (initialized_) {
        core::log(core::LogLevel::Warn, "UIManager already initialized");
        return true;
    }
    
    core::log(core::LogLevel::Info, "Initializing UIManager...");
    
    // Initialize overlay visibility array
    overlay_visibility_.resize(static_cast<size_t>(OverlayType::KeyBindings) + 1, false);
    overlay_modal_.resize(static_cast<size_t>(OverlayType::KeyBindings) + 1, false);
    
    // HUD is visible by default
    overlay_visibility_[static_cast<size_t>(OverlayType::HUD)] = true;
    
    // Create overlays
    overlays_.resize(static_cast<size_t>(OverlayType::KeyBindings) + 1);
    overlays_[static_cast<size_t>(OverlayType::HUD)] = std::make_unique<HUD>();
    overlays_[static_cast<size_t>(OverlayType::Settings)] = std::make_unique<SettingsMenu>();
    overlays_[static_cast<size_t>(OverlayType::KeyBindings)] = std::make_unique<KeyBindingsMenu>();
    
    // Set modal states
    overlay_modal_[static_cast<size_t>(OverlayType::Settings)] = true;
    overlay_modal_[static_cast<size_t>(OverlayType::KeyBindings)] = true;
    
    // Initialize ImGui if GLFW window is provided
    if (glfwWindow) {
        initializeImGui(static_cast<GLFWwindow*>(glfwWindow));
    }
    
    initialized_ = true;
    core::log(core::LogLevel::Info, "UIManager initialized successfully");
    return true;
}

void UIManager::shutdown() {
    if (!initialized_) return;
    
    core::log(core::LogLevel::Info, "Shutting down UIManager...");
    
    // Clear overlays first
    overlays_.clear();
    overlay_visibility_.clear();
    overlay_modal_.clear();
    overlay_callback_ = nullptr;
    
    // Shutdown ImGui last
#ifdef VOXEL_WITH_GL
    shutdownImGui();
#endif
    
    initialized_ = false;
    core::log(core::LogLevel::Info, "UIManager shutdown complete");
}

void UIManager::beginFrame(float deltaTime) {
    if (!initialized_) return;
    
#ifdef VOXEL_WITH_GL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif
    
    // Update all visible overlays
    for (size_t i = 0; i < overlays_.size(); ++i) {
        if (overlays_[i] && overlay_visibility_[i]) {
            overlays_[i]->update(deltaTime);
        }
    }
}

void UIManager::endFrame() {
    if (!initialized_) return;
    
    // Render all visible overlays
    for (size_t i = 0; i < overlays_.size(); ++i) {
        if (overlays_[i] && overlay_visibility_[i]) {
            overlays_[i]->render();
        }
    }
    
#ifdef VOXEL_WITH_GL
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}

void UIManager::showOverlay(OverlayType type) {
    size_t index = static_cast<size_t>(type);
    if (index >= overlay_visibility_.size()) return;
    
    // If this overlay is modal, set game state to paused
    if (overlay_modal_[index]) {
        setGameState(GameState::Paused);
    }
    
    overlay_visibility_[index] = true;
    if (overlays_[index]) {
        overlays_[index]->show();
    }
    
    if (overlay_callback_) {
        overlay_callback_(type, true);
    }
}

void UIManager::hideOverlay(OverlayType type) {
    size_t index = static_cast<size_t>(type);
    if (index >= overlay_visibility_.size()) return;
    
    overlay_visibility_[index] = false;
    if (overlays_[index]) {
        overlays_[index]->hide();
    }
    
    // If this was a modal overlay, check if we should resume the game
    if (overlay_modal_[index] && !isModalActive()) {
        setGameState(GameState::Running);
    }
    
    if (overlay_callback_) {
        overlay_callback_(type, false);
    }
}

void UIManager::toggleOverlay(OverlayType type) {
    if (isOverlayVisible(type)) {
        hideOverlay(type);
    } else {
        showOverlay(type);
    }
}

bool UIManager::isOverlayVisible(OverlayType type) const {
    size_t index = static_cast<size_t>(type);
    return index < overlay_visibility_.size() && overlay_visibility_[index];
}

void UIManager::setModal(OverlayType type, bool modal) {
    size_t index = static_cast<size_t>(type);
    if (index >= overlay_modal_.size()) return;
    
    overlay_modal_[index] = modal;
    
    if (modal && isOverlayVisible(type)) {
        active_modal_ = type;
    }
}

bool UIManager::isModalActive() const {
    for (size_t i = 0; i < overlay_modal_.size(); ++i) {
        if (overlay_modal_[i] && overlay_visibility_[i]) {
            return true;
        }
    }
    return false;
}

bool UIManager::shouldBlockInput() const {
    return isModalActive();
}

void UIManager::setGameState(GameState state) {
    game_state_ = state;
    std::string stateStr = (state == GameState::Running ? "Running" : "Paused");
    core::log(core::LogLevel::Info, "Game state changed to: " + stateStr);
}

void UIManager::loadSettings() {
    // Load settings from config
    const auto& uiConfig = config::Config::instance().ui();
    
    // Apply UI scale
#ifdef VOXEL_WITH_GL
    if (ImGui::GetCurrentContext()) {
        ImGui::GetIO().FontGlobalScale = uiConfig.scale;
    }
#endif
}

void UIManager::saveSettings() {
    // Settings are saved automatically by individual overlays
    // This is a placeholder for any global UI settings
}

void UIManager::applySettings() {
    loadSettings();
}

void UIManager::setOverlayEventCallback(OverlayEventCallback callback) {
    overlay_callback_ = callback;
}

#ifdef VOXEL_WITH_GL
void UIManager::initializeImGui(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup Dear ImGui style
    if (config::Config::instance().ui().theme == "dark") {
        ImGui::StyleColorsDark();
    } else {
        ImGui::StyleColorsLight();
    }
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void UIManager::shutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
#endif

} // namespace ui
