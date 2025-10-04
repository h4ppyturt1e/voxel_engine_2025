#include "settings_menu.hpp"
#include "ui_manager.hpp"

#ifdef VOXEL_WITH_GL
#include <imgui.h>
#include <GLFW/glfw3.h>
#endif

#include "../config/config.hpp"
#include "../config/config_manager.hpp"
#include "../core/logging.hpp"
#include "../input/input_manager.hpp"

#include <sstream>

namespace ui {

SettingsMenu::SettingsMenu() : Overlay(OverlayType::Settings, "Settings", true) {
    loadSettings();
}

void SettingsMenu::render() {
    if (!isVisible()) return;
    
#ifdef VOXEL_WITH_GL
    // Create a centered modal window
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_Always);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    
    if (ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        
        if (ImGui::BeginTabBar("SettingsTabs")) {
            
            // Graphics Tab
            if (ImGui::BeginTabItem("Graphics")) {
                renderGraphicsSettings();
                ImGui::EndTabItem();
            }
            
            // Audio Tab
            if (ImGui::BeginTabItem("Audio")) {
                renderAudioSettings();
                ImGui::EndTabItem();
            }
            
            // UI Tab
            if (ImGui::BeginTabItem("UI")) {
                renderUISettings();
                ImGui::EndTabItem();
            }
            
            // Controls Tab
            if (ImGui::BeginTabItem("Controls")) {
                renderControls();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        ImGui::Separator();
        
        // Action buttons
        ImGui::Spacing();
        if (ImGui::Button("Apply", ImVec2(100, 0))) {
            applySettings();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset", ImVec2(100, 0))) {
            loadSettings();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(100, 0))) {
            hide();
        }
        
        if (settings_changed_) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "* Unsaved changes");
        }
    }
    ImGui::End();
#endif
}

void SettingsMenu::update(float deltaTime) {
    // ESC handling is done globally by UIManager
}

void SettingsMenu::renderGraphicsSettings() {
#ifdef VOXEL_WITH_GL
    ImGui::Text("Graphics Settings");
    ImGui::Separator();
    
    // VSync
    if (ImGui::Checkbox("VSync", &settings_.vsync)) {
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // Resolution
    ImGui::Text("Resolution:");
    ImGui::InputInt("Width", &settings_.resolution_width, 1, 100);
    ImGui::InputInt("Height", &settings_.resolution_height, 1, 100);
    if (ImGui::Button("Apply Resolution")) {
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // Quality
    ImGui::Text("Quality:");
    const char* quality_items[] = { "Low", "Medium", "High", "Ultra" };
    int current_quality = 0;
    if (settings_.quality == "low") current_quality = 0;
    else if (settings_.quality == "medium") current_quality = 1;
    else if (settings_.quality == "high") current_quality = 2;
    else if (settings_.quality == "ultra") current_quality = 3;
    
    if (ImGui::Combo("##Quality", &current_quality, quality_items, 4)) {
        switch (current_quality) {
            case 0: settings_.quality = "low"; break;
            case 1: settings_.quality = "medium"; break;
            case 2: settings_.quality = "high"; break;
            case 3: settings_.quality = "ultra"; break;
        }
        settings_changed_ = true;
    }
#endif
}

void SettingsMenu::renderAudioSettings() {
#ifdef VOXEL_WITH_GL
    ImGui::Text("Audio Settings");
    ImGui::Separator();
    
    // Master Volume
    ImGui::Text("Master Volume: %.0f%%", settings_.master_volume * 100.0f);
    if (ImGui::SliderFloat("##MasterVolume", &settings_.master_volume, 0.0f, 1.0f)) {
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // SFX Volume
    ImGui::Text("SFX Volume: %.0f%%", settings_.sfx_volume * 100.0f);
    if (ImGui::SliderFloat("##SFXVolume", &settings_.sfx_volume, 0.0f, 1.0f)) {
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // Music Volume
    ImGui::Text("Music Volume: %.0f%%", settings_.music_volume * 100.0f);
    if (ImGui::SliderFloat("##MusicVolume", &settings_.music_volume, 0.0f, 1.0f)) {
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // Audio Device
    ImGui::Text("Audio Device:");
    char device_buffer[256];
    strncpy(device_buffer, settings_.device.c_str(), sizeof(device_buffer) - 1);
    device_buffer[sizeof(device_buffer) - 1] = '\0';
    
    if (ImGui::InputText("##AudioDevice", device_buffer, sizeof(device_buffer))) {
        settings_.device = device_buffer;
        settings_changed_ = true;
    }
#endif
}

void SettingsMenu::renderUISettings() {
#ifdef VOXEL_WITH_GL
    ImGui::Text("UI Settings");
    ImGui::Separator();
    
    // Mouse Sensitivity
    ImGui::Text("Mouse Sensitivity: %.3f", settings_.mouse_sensitivity);
    if (ImGui::SliderFloat("##MouseSensitivity", &settings_.mouse_sensitivity, 0.001f, 0.1f, "%.3f")) {
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // UI Theme
    ImGui::Text("Theme:");
    const char* theme_items[] = { "Dark", "Light" };
    int current_theme = (settings_.theme == "dark") ? 0 : 1;
    
    if (ImGui::Combo("##Theme", &current_theme, theme_items, 2)) {
        settings_.theme = (current_theme == 0) ? "dark" : "light";
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // UI Scale
    ImGui::Text("UI Scale: %.1f", settings_.scale);
    if (ImGui::SliderFloat("##UIScale", &settings_.scale, 0.5f, 2.0f)) {
        settings_changed_ = true;
    }
#endif
}

void SettingsMenu::renderControls() {
#ifdef VOXEL_WITH_GL
    ImGui::Text("Controls");
    ImGui::Separator();
    
    ImGui::Text("Key Bindings:");
    ImGui::Text("ESC - Toggle Menu");
    ImGui::Text("F3 - Toggle Debug");
    ImGui::Text("F4 - Toggle Mouse Lock");
    ImGui::Text("F5 - Toggle VSync");
    ImGui::Text("R - Recenter Camera");
    ImGui::Text("F - Toggle Wireframe");
    ImGui::Text("WASD - Move");
    ImGui::Text("Space - Move Up");
    ImGui::Text("Ctrl - Move Down");
    ImGui::Text("Shift - Fast Movement");
    ImGui::Text("Mouse - Look");
    ImGui::Text("Left Click - Break Block");
    ImGui::Text("Right Click - Place Block");
    
    ImGui::Spacing();
    
    if (ImGui::Button("Open Key Bindings Menu", ImVec2(200, 0))) {
        // TODO: Open key bindings menu
        core::log(core::LogLevel::Info, "Key bindings menu not yet implemented");
    }
#endif
}

void SettingsMenu::saveSettings() {
    // Save to config file
    config::ConfigManager& configManager = config::ConfigManager::instance();
    std::string configPath = configManager.getConfigPath("engine.ini");
    
    // TODO: Implement config file writing
    // For now, just log the settings
    core::log(core::LogLevel::Info, "Settings saved (not yet implemented)");
    
    settings_changed_ = false;
}

void SettingsMenu::loadSettings() {
    const auto& graphicsConfig = config::Config::instance().graphics();
    const auto& audioConfig = config::Config::instance().audio();
    const auto& uiConfig = config::Config::instance().ui();
    
    settings_.vsync = graphicsConfig.vsync;
    settings_.resolution_width = graphicsConfig.resolution_width;
    settings_.resolution_height = graphicsConfig.resolution_height;
    settings_.quality = graphicsConfig.quality;
    settings_.master_volume = audioConfig.master_volume;
    settings_.sfx_volume = audioConfig.sfx_volume;
    settings_.music_volume = audioConfig.music_volume;
    settings_.device = audioConfig.device;
    settings_.mouse_sensitivity = uiConfig.mouse_sensitivity;
    settings_.theme = uiConfig.theme;
    settings_.scale = uiConfig.scale;
    
    settings_changed_ = false;
}

void SettingsMenu::applySettings() {
    saveSettings();
    
    // Apply immediate settings
    // TODO: Apply graphics settings like resolution, VSync
    // TODO: Apply audio settings
    // TODO: Apply UI theme and scale
    
    core::log(core::LogLevel::Info, "Settings applied");
}

} // namespace ui
