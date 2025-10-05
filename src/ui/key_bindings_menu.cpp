#include "key_bindings_menu.hpp"
#include "ui_manager.hpp"

#ifdef VOXEL_WITH_GL
#include <imgui.h>
#endif

#include "../input/input_manager.hpp"
#include "../core/logging.hpp"

namespace ui {

KeyBindingsMenu::KeyBindingsMenu() : Overlay(OverlayType::KeyBindings, "Key Bindings", true) {
    loadKeyBindings();
}

void KeyBindingsMenu::render() {
    if (!isVisible()) return;
    
#ifdef VOXEL_WITH_GL
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Always);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    
    if (ImGui::Begin("Key Bindings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        
        ImGui::Text("Key Bindings Configuration");
        ImGui::Separator();
        
        // Movement
        ImGui::Text("Movement:");
        renderKeyBinding("Move Forward", "W");
        renderKeyBinding("Move Backward", "S");
        renderKeyBinding("Move Left", "A");
        renderKeyBinding("Move Right", "D");
        renderKeyBinding("Move Up", "SPACE");
        renderKeyBinding("Move Down", "CTRL");
        renderKeyBinding("Fast Movement", "SHIFT");
        
        ImGui::Spacing();
        
        // Camera
        ImGui::Text("Camera:");
        renderKeyBinding("Recenter Camera", "R");
        renderKeyBinding("Toggle Mouse Lock", "F4");
        
        ImGui::Spacing();
        
        // Actions
        ImGui::Text("Actions:");
        renderKeyBinding("Break Block", "MOUSE_LEFT");
        renderKeyBinding("Place Block", "MOUSE_RIGHT");
        
        ImGui::Spacing();
        
        // UI
        ImGui::Text("UI:");
        renderKeyBinding("Toggle Menu", "ESCAPE");
        renderKeyBinding("Toggle Debug", "F3");
        renderKeyBinding("Toggle Wireframe", "F");
        renderKeyBinding("Toggle VSync", "F5");
        
        ImGui::Separator();
        
        if (ImGui::Button("Reset to Defaults", ImVec2(150, 0))) {
            core::log(core::LogLevel::Info, "Reset key bindings to defaults");
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(100, 0))) {
            UIManager::instance().hideOverlay(getType());
        }
        
        if (is_remapping_) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Press a key to assign to: %s", remapping_action_.c_str());
        }
    }
    ImGui::End();
#endif
}

void KeyBindingsMenu::update(float deltaTime) {
    // ESC handling is done globally by UIManager
}

void KeyBindingsMenu::renderKeyBinding(const std::string& action, const std::string& currentKey) {
#ifdef VOXEL_WITH_GL
    ImGui::Text("%s:", action.c_str());
    ImGui::SameLine(150);
    
    std::string buttonLabel = currentKey;
    if (is_remapping_ && remapping_action_ == action) {
        buttonLabel = "...";
    }
    
    if (ImGui::Button(buttonLabel.c_str(), ImVec2(100, 0))) {
        if (!is_remapping_) {
            is_remapping_ = true;
            remapping_action_ = action;
            core::log(core::LogLevel::Info, "Started remapping: " + action);
        }
    }
    
    // TODO: Handle key input for remapping
    if (is_remapping_ && remapping_action_ == action) {
        // This would be implemented to capture the next key press
        // For now, just a placeholder
    }
#endif
}

void KeyBindingsMenu::saveKeyBindings() {
    // TODO: Save key bindings to input.ini
    core::log(core::LogLevel::Info, "Key bindings saved (not yet implemented)");
}

void KeyBindingsMenu::loadKeyBindings() {
    // TODO: Load key bindings from input.ini
    core::log(core::LogLevel::Info, "Key bindings loaded (not yet implemented)");
}

} // namespace ui
