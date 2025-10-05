#include "hud.hpp"
#include "ui_manager.hpp"

#ifdef VOXEL_WITH_GL
#include <imgui.h>
#include <GLFW/glfw3.h>
#endif

#include "../input/input_manager.hpp"
#include "../core/logging.hpp"

namespace ui {

HUD::HUD() : Overlay(OverlayType::HUD, "HUD", false) {
    show();
}

void HUD::render() {
    if (!isVisible()) return;
    
#ifdef VOXEL_WITH_GL
    // Set window flags for HUD
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | 
                                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                                   ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    
    // FPS counter in top-left
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    if (ImGui::Begin("FPS", nullptr, window_flags)) {
        ImGui::Text("FPS: %.1f", fps_);
        
        // Show pause indicator if game is paused
        if (UIManager::instance().isGamePaused()) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "PAUSED");
        }
    }
    ImGui::End();
    
    // Crosshair in center - transparent window
    ImGuiWindowFlags crosshair_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | 
                                      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                                      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f - 8, ImGui::GetIO().DisplaySize.y * 0.5f - 8), ImGuiCond_Always);
    if (ImGui::Begin("Crosshair", nullptr, crosshair_flags)) {
        ImGui::Text("+");
    }
    ImGui::End();
    
    // Debug info if enabled
    if (show_debug_) {
        renderDebugInfo();
    }
#endif
}

void HUD::update(float deltaTime) {
    // Update FPS counter
    frame_count_++;
    fps_timer_ += deltaTime;
    
    if (fps_timer_ >= 1.0f) {
        fps_ = frame_count_ / fps_timer_;
        frame_count_ = 0;
        fps_timer_ = 0.0f;
    }
    
    // Toggle debug with F3
    static bool prev_f3 = false;
    bool cur_f3 = input::InputManager::instance().isActionPressed(input::Action::ToggleDebug);
    if (cur_f3 && !prev_f3) {
        show_debug_ = !show_debug_;
    }
    prev_f3 = cur_f3;
}

void HUD::renderCrosshair() {
    // Crosshair is rendered in main render() method
}

void HUD::renderFPS() {
    // FPS is rendered in main render() method
}

void HUD::renderDebugInfo() {
#ifdef VOXEL_WITH_GL
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | 
                                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                                   ImGuiWindowFlags_NoNav;
    
    ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_Always);
    if (ImGui::Begin("Debug", nullptr, window_flags)) {
        ImGui::Text("Debug Mode Active");
        ImGui::Separator();
        ImGui::Text("Controls:");
        ImGui::Text("F3 - Toggle Debug");
        ImGui::Text("ESC - Toggle Menu");
        ImGui::Text("F4 - Toggle Mouse Lock");
        ImGui::Text("F5 - Toggle VSync");
    }
    ImGui::End();
#endif
}

} // namespace ui
