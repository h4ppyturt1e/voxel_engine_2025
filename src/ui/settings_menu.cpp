#include "settings_menu.hpp"
#include "ui_manager.hpp"

#ifdef VOXEL_WITH_GL
#include <imgui.h>
#include <GLFW/glfw3.h>
#endif

#include "../config/config.hpp"
#include "../config/config_manager.hpp"
#include "../config/ini_parser.hpp"
#include "../core/logging.hpp"
#include "../input/input_manager.hpp"
#include "../input/key_constants.hpp"

#include <sstream>
#include <fstream>
#include <filesystem>

namespace ui {

SettingsMenu::SettingsMenu() : Overlay(OverlayType::Settings, "Settings", true) {
    loadSettings();
    
    // Set up show callback to reset temp settings when menu is opened
    setOnShowCallback([this]() {
        resetTempSettings();
    });
}

void SettingsMenu::render() {
    if (!isVisible()) return;
    
#ifdef VOXEL_WITH_GL
    // Create a centered modal window; size scales with UI scale and clamps to display
    ImVec2 display = ImGui::GetIO().DisplaySize;
    float scale = config::Config::instance().ui().scale;
    float baseW = 600.0f, baseH = 500.0f;
    float w = baseW * scale;
    float h = baseH * scale;
    // Clamp to not exceed display size (leave small margin)
    const float margin = 20.0f;
    if (w > display.x - margin) w = display.x - margin;
    if (h > display.y - margin) h = display.y - margin;
    if (w < 100.0f) w = 100.0f;
    if (h < 100.0f) h = 100.0f;
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    
    if (ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        
        if (ImGui::BeginTabBar("SettingsTabs")) {
            
            // Graphics Tab
            if (ImGui::BeginTabItem("Graphics")) {
                renderGraphicsSettings();
                ImGui::EndTabItem();
            }
            
            // Audio removed
            
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
        ImGui::Spacing();
        ImGui::Text(warning_message.c_str());
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
            UIManager::instance().hideOverlay(getType());
        }
        
        if (settings_changed_) {
            ImGui::SameLine();
            // Use accessible amber that works on light and dark
            ImGui::TextColored(ImVec4(0.55f, 0.38f, 0.00f, 1.0f), "* Unsaved changes");
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
    if (ImGui::Checkbox("VSync", &temp_settings_.vsync)) {
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // Resolution simplified: force default shown value 1280x720 until Apply
    temp_settings_.resolution_width = 1280;
    temp_settings_.resolution_height = 720;
    ImGui::Text("Resolution: 1280 x 720 (fixed)");
    if (ImGui::Checkbox("Fullscreen", &temp_settings_.fullscreen)) { settings_changed_ = true; }
    // Removed per-section Apply; master Apply button handles it
    // Persist fullscreen into settings on apply path below
    
    ImGui::Spacing();
    
    // Quality
    ImGui::Text("Quality:");
    const char* quality_items[] = { "Low", "Medium", "High", "Ultra" };
    int current_quality = 0;
    if (temp_settings_.quality == "low") current_quality = 0;
    else if (temp_settings_.quality == "medium") current_quality = 1;
    else if (temp_settings_.quality == "high") current_quality = 2;
    else if (temp_settings_.quality == "ultra") current_quality = 3;
    
    if (ImGui::Combo("##Quality", &current_quality, quality_items, 4)) {
        switch (current_quality) {
            case 0: temp_settings_.quality = "low"; break;
            case 1: temp_settings_.quality = "medium"; break;
            case 2: temp_settings_.quality = "high"; break;
            case 3: temp_settings_.quality = "ultra"; break;
        }
        settings_changed_ = true;
    }
#endif
}

// Audio UI removed

void SettingsMenu::renderUISettings() {
#ifdef VOXEL_WITH_GL
    ImGui::Text("UI Settings");
    ImGui::Separator();
    
    // Mouse Sensitivity: 0-100% mapped to 0.001-0.08
    if (temp_settings_.mouse_sensitivity <= 0.0001f) temp_settings_.mouse_sensitivity = settings_.mouse_sensitivity;
    float sensMin = 0.001f, sensMax = 0.08f;
    temp_settings_.mouse_sensitivity_percent = (temp_settings_.mouse_sensitivity - sensMin) / (sensMax - sensMin) * 100.0f;
    if (temp_settings_.mouse_sensitivity_percent < 0.0f) temp_settings_.mouse_sensitivity_percent = 0.0f;
    if (temp_settings_.mouse_sensitivity_percent > 100.0f) temp_settings_.mouse_sensitivity_percent = 100.0f;
    ImGui::Text("Mouse Sensitivity: %.0f%%", temp_settings_.mouse_sensitivity_percent);
    if (ImGui::SliderFloat("##MouseSensitivityPercent", &temp_settings_.mouse_sensitivity_percent, 0.0f, 100.0f, "%.0f%%")) {
        settings_changed_ = true;
        temp_settings_.mouse_sensitivity = sensMin + (sensMax - sensMin) * (temp_settings_.mouse_sensitivity_percent / 100.0f);
    }
    
    ImGui::Spacing();

    // Custom font toggle
    if (ImGui::Checkbox("Use custom font", &temp_settings_.font_enabled)) { settings_changed_ = true; }
    
    // Font size (default font)
    ImGui::Text("Font Size: %.0f", temp_settings_.font_size);
    if (ImGui::SliderFloat("##UIFontSize", &temp_settings_.font_size, 10.0f, 36.0f, "%.0f")) {
        settings_changed_ = true;
    }

    // Crosshair
    if (ImGui::Checkbox("Enable Crosshair", &temp_settings_.crosshair_enabled)) {
        settings_changed_ = true;
    }
    // Crosshair size removed; percentage only
    ImGui::Text("Crosshair Size: %.0f%%", temp_settings_.crosshair_percent);
    if (ImGui::SliderFloat("##CrosshairPercent", &temp_settings_.crosshair_percent, 0.0f, 100.0f, "%.0f%%")) {
        settings_changed_ = true;
    }
    
    // UI Theme
    ImGui::Text("Theme:");
    const char* theme_items[] = { "Dark", "Light" };
    int current_theme = (temp_settings_.theme == "dark") ? 0 : 1;
    
    if (ImGui::Combo("##Theme", &current_theme, theme_items, 2)) {
        temp_settings_.theme = (current_theme == 0) ? "dark" : "light";
        settings_changed_ = true;
    }
    
    ImGui::Spacing();
    
    // UI Scale
    ImGui::Text("UI Scale: %.1f", temp_settings_.scale);
    if (ImGui::SliderFloat("##UIScale", &temp_settings_.scale, 0.5f, 2.0f)) {
        settings_changed_ = true;
    }
#endif
}

void SettingsMenu::renderControls() {
    #ifdef VOXEL_WITH_GL
    std::vector<std::pair<input::Action, std::string>> controls = {
        // {input::Action::ToggleMenu, "Toggle Menu"},   //idt we want this one to change?
        {input::Action::ToggleDebug, "Toggle Debug"},
        {input::Action::ToggleMouseLock, "Toggle Mouse Lock"},
        {input::Action::ToggleVSync, "Toggle VSync"},
        {input::Action::RecenterCamera, "Recenter Camera"},
        {input::Action::ToggleWireframe, "Toggle Wireframe"},
        {input::Action::MoveForward, "Move Forward"},
        {input::Action::MoveBackward, "Move Backward"},
        {input::Action::MoveLeft, "Move Left"},
        {input::Action::MoveRight, "Move Right"},
        {input::Action::MoveUp, "Move Up"},
        {input::Action::MoveDown, "Move Down"},
        {input::Action::FastMovement, "Fast Movement"},
        {input::Action::BreakBlock, "Break Block"},
        {input::Action::PlaceBlock, "Place Block"}
    };

    ImGui::Text("Controls");
    ImGui::Separator();

    ImGui::Text("Key Bindings:");

    ImGui::Columns(2, nullptr, false);

    for (auto& [actionEnum, controlName] : controls) {
        ImGui::Text("%s", controlName.c_str());
        ImGui::NextColumn();

        // add ##controlName so button labels are guaranteed to be unique
        std::string buttonLabel = temp_settings_.key_bindings[actionEnum] + "##" + controlName;

        if (ImGui::Button(buttonLabel.c_str(), ImVec2(200, 0)) && buttonLabel.find("Press any key") == std::string::npos) {
            temp_settings_.key_bindings[actionEnum] = "Press any key";

            input::InputManager::instance().waitForNextKey([this, actionEnum, controlName](int key) {
                // (not implemented) handle illegal key bindings
                // (not implemented) handle escape to cancel

                // handle duplicate key bindings
                for (const auto& [otherAction, keyName] : temp_settings_.key_bindings) {
                    if (otherAction != actionEnum && keyName != "Unbound") {
                        core::log(core::LogLevel::Debug, "Checking for duplicate key binding: " + keyName);
                        int otherKeyCode = input::keyNameToCode(keyName);
                        if (otherKeyCode == key) {
                            // Key is already bound to another action
                            temp_settings_.key_bindings[otherAction] = "Unbound";                                  
                        }
                    }
                }

                // set new binding
                std::string keyName = input::keyCodeToName(key);
                temp_settings_.key_bindings[actionEnum] = keyName;
                settings_changed_ = true;
                return true;
            });
        }

        ImGui::NextColumn();
    }

    ImGui::Columns(1);

#endif
}

void SettingsMenu::saveSettings() {
    // Save to config file
    config::ConfigManager& configManager = config::ConfigManager::instance();
    std::string configPath = configManager.getConfigPath("engine.ini");
    
    try {
        std::ofstream file(configPath);
        if (!file.is_open()) {
            core::log(core::LogLevel::Error, "Failed to open config file for writing: " + configPath);
            return;
        }
        
        // Write header
        file << "# Voxel Engine 2025 Configuration\n";
        file << "chunk.size_x=8\n";
        file << "chunk.size_y=8\n";
        file << "chunk.size_z=8\n";
        file << "log.level=debug\n";
        file << "log.file=logs/engine.log\n\n";
        
        // Write graphics section
        file << "[graphics]\n";
        file << "vsync=" << (settings_.vsync ? "true" : "false") << "\n";
        file << "resolution_width=" << settings_.resolution_width << "\n";
        file << "resolution_height=" << settings_.resolution_height << "\n";
        file << "fullscreen=" << (settings_.fullscreen ? "true" : "false") << "\n";
        file << "# graphics.quality=" << settings_.quality << "\n\n";
        
        // Write UI section
        file << "[ui]\n";
        file << "mouse_sensitivity=" << settings_.mouse_sensitivity << "\n";
        file << "crosshair_enabled=" << (settings_.crosshair_enabled ? "true" : "false") << "\n";
        file << "crosshair_percent=" << settings_.crosshair_percent << "\n";
        file << "theme=" << settings_.theme << "\n";
        file << "scale=" << settings_.scale << "\n\n";
        
        // Audio section removed
        file.close();
        core::log(core::LogLevel::Info, "Settings saved to: " + configPath);
        
    } catch (const std::exception& e) {
        core::log(core::LogLevel::Error, "Failed to save settings: " + std::string(e.what()));
        return;
    }

    configPath = configManager.getConfigPath("input.ini");
    try {
        std::ofstream file(configPath);
        if (!file.is_open()) {
            core::log(core::LogLevel::Error, "Failed to open config file for writing: " + configPath);
            return;
        }
        
        // Write header
        file << "# Input Configuration File\n# Format: action=key_name\n";
        file << "[actions]\n";
        for (const auto& [action, keyName] : settings_.key_bindings) {
            file << input::InputManager::instance().actionToString(action) << "=" << keyName << "\n";
        }

        file.close();
        core::log(core::LogLevel::Info, "Settings saved to: " + configPath);

    } catch (const std::exception& e) {
        core::log(core::LogLevel::Error, "Failed to save settings: " + std::string(e.what()));
        return;
    }
    
    settings_changed_ = false;
}

void SettingsMenu::loadSettings() {
    const auto& graphicsConfig = config::Config::instance().graphics();
    const auto& uiConfig = config::Config::instance().ui();

    // Load applied settings
    settings_.vsync = graphicsConfig.vsync;
    settings_.resolution_width = graphicsConfig.resolution_width;
    settings_.resolution_height = graphicsConfig.resolution_height;
    settings_.quality = graphicsConfig.quality;
    settings_.mouse_sensitivity = uiConfig.mouse_sensitivity;
    settings_.theme = uiConfig.theme;
    settings_.scale = uiConfig.scale;
    settings_.crosshair_enabled = uiConfig.crosshair_enabled;
    // size removed
    settings_.crosshair_percent = uiConfig.crosshair_percent;
    settings_.fullscreen = config::Config::instance().graphics().fullscreen;
    // Load font_size from theme.ini
    {
        config::IniParser parser;
        if (parser.parseFile(config::ConfigManager::instance().getConfigPath("theme.ini"))) {
            auto uiSec = parser.section("ui");
            auto its = uiSec.find("font_size");
            if (its != uiSec.end()) {
                try { settings_.font_size = std::stof(its->second); } catch (...) { settings_.font_size = 18.0f; }
            } else { settings_.font_size = 18.0f; }
        } else { settings_.font_size = 18.0f; }
    }

    for (const auto& [action, keycode] : input::InputManager::instance().getInputBindings()) {
        settings_.key_bindings.emplace(action, input::keyCodeToName(keycode));
    }

    // Initialize temp settings to same values
    SettingsMenu::resetTempSettings();
}

void SettingsMenu::resetTempSettings() {
    // Reset temp settings to current applied settings
    temp_settings_.vsync = settings_.vsync;
    temp_settings_.resolution_width = settings_.resolution_width;
    temp_settings_.resolution_height = settings_.resolution_height;
    temp_settings_.quality = settings_.quality;
    temp_settings_.quality = settings_.quality;
    temp_settings_.mouse_sensitivity = settings_.mouse_sensitivity;
    temp_settings_.mouse_sensitivity_percent = (settings_.mouse_sensitivity - 0.0001f) / (0.08f - 0.0001f) * 100.0f;
    temp_settings_.theme = settings_.theme;
    temp_settings_.mouse_sensitivity = settings_.mouse_sensitivity;
    temp_settings_.mouse_sensitivity_percent = (settings_.mouse_sensitivity - 0.0001f) / (0.08f - 0.0001f) * 100.0f;
    temp_settings_.theme = settings_.theme;
    temp_settings_.scale = settings_.scale;

    temp_settings_.key_bindings.clear();
    for (const auto& kv : settings_.key_bindings) {
        temp_settings_.key_bindings.emplace(kv.first, kv.second);
    }

    settings_changed_ = false;
    warning_message = "";
    
    core::log(core::LogLevel::Info, "Temp settings reset to applied values");
}

void SettingsMenu::applySettings() {
    // Copy temp settings to applied settings
    for (const auto& kv : temp_settings_.key_bindings) {
        if (kv.second == "Unbound") {
            warning_message = "Some keys are unbound. Please bind all actions before applying.";
            return;
        }
        settings_.key_bindings[kv.first] = kv.second;
    }
    warning_message = "";

    settings_.vsync = temp_settings_.vsync;
    settings_.resolution_width = temp_settings_.resolution_width;
    settings_.resolution_height = temp_settings_.resolution_height;
    settings_.quality = temp_settings_.quality;
    settings_.mouse_sensitivity = temp_settings_.mouse_sensitivity;
    settings_.theme = temp_settings_.theme;
    settings_.scale = temp_settings_.scale;
    settings_.crosshair_enabled = temp_settings_.crosshair_enabled;
    settings_.crosshair_percent = temp_settings_.crosshair_percent;
    settings_.fullscreen = temp_settings_.fullscreen;
    settings_.font_size = temp_settings_.font_size;
    settings_.font_enabled = temp_settings_.font_enabled;

    saveSettings();
    
    // Apply immediate settings
    try {
        // Update the config singleton with new values
        config::Config& config = config::Config::instance();
        
        // Apply graphics settings
        config.graphics().vsync = settings_.vsync;
        config.graphics().resolution_width = settings_.resolution_width;
        config.graphics().resolution_height = settings_.resolution_height;
        config.graphics().fullscreen = settings_.fullscreen;
        // config.graphics().quality = settings_.quality; // Not implemented
        
        // Apply audio settings (none implemented yet)
        // config.audio().master_volume = settings_.master_volume; // Not implemented
        // config.audio().sfx_volume = settings_.sfx_volume; // Not implemented
        // config.audio().music_volume = settings_.music_volume; // Not implemented
        // config.audio().device = settings_.device; // Not implemented
        
        // Apply UI settings
        config.ui().mouse_sensitivity = settings_.mouse_sensitivity;
        config.ui().crosshair_enabled = settings_.crosshair_enabled;
        config.ui().crosshair_percent = settings_.crosshair_percent;
        config.ui().theme = settings_.theme;
        config.ui().scale = settings_.scale;
        // Persist font settings to theme.ini [ui]
        {
            std::string themePath = config::ConfigManager::instance().getConfigPath("theme.ini");
            std::ifstream in(themePath);
            std::vector<std::string> lines; std::string line;
            while (std::getline(in, line)) lines.push_back(line);
            in.close();
            if (lines.empty()) lines.push_back("[ui]");
            auto trim = [](std::string s){ while(!s.empty() && (s.back()=='\r'||s.back()=='\n')) s.pop_back(); return s; };
            int uiStart=-1, uiEnd=(int)lines.size();
            for (int i=0;i<(int)lines.size();++i){ std::string t=lines[i]; if (trim(t)=="[ui]"){ uiStart=i; break; } }
            if (uiStart==-1){ lines.push_back("[ui]"); lines.push_back(std::string("font_size=")+std::to_string((int)temp_settings_.font_size)); lines.push_back(std::string("font_enabled=")+(temp_settings_.font_enabled?"true":"false")); }
            else {
                for (int i=uiStart+1;i<(int)lines.size();++i){ std::string t=trim(lines[i]); if (!t.empty() && t.front()=='[' && t.back()==']'){ uiEnd=i; break; } }
                bool foundSize=false, foundEnabled=false;
                for (int i=uiStart+1;i<uiEnd;++i){ std::string t=trim(lines[i]); if (t.rfind("font_size=",0)==0){ lines[i]=std::string("font_size=")+std::to_string((int)temp_settings_.font_size); foundSize=true; } if (t.rfind("font_enabled=",0)==0){ lines[i]=std::string("font_enabled=")+(temp_settings_.font_enabled?"true":"false"); foundEnabled=true; } }
                if (!foundSize) lines.insert(lines.begin()+uiEnd, std::string("font_size=")+std::to_string((int)temp_settings_.font_size));
                if (!foundEnabled) lines.insert(lines.begin()+uiEnd, std::string("font_enabled=")+(temp_settings_.font_enabled?"true":"false"));
            }
            std::ofstream out(themePath, std::ios::trunc); for (auto& l:lines) out<<l<<"\n";
        }
        ui::UIManager::instance().markThemeDirty();
        
        // Apply mouse sensitivity to InputManager immediately
        input::InputManager& inputManager = input::InputManager::instance();
        inputManager.setMouseSensitivity(settings_.mouse_sensitivity);

        // Apply key bindings to InputManager
        std::string inputPath = config::ConfigManager::instance().getConfigPath("input.ini");
        inputManager.loadConfig(inputPath);

        // Apply VSync and UI appearance immediately, and update window size
        ui::UIManager::instance().setVSync(settings_.vsync);
        // Defer theme/font changes to start of next frame to avoid GL crashes
        ui::UIManager::instance().applySettings();
        ui::UIManager::instance().markThemeDirty();
        ui::UIManager::instance().setWindowSize(settings_.resolution_width, settings_.resolution_height);
        ui::UIManager::instance().setFullscreen(config.graphics().fullscreen);

        core::log(core::LogLevel::Info, "Settings applied successfully");
        
    } catch (const std::exception& e) {
        core::log(core::LogLevel::Error, "Failed to apply settings: " + std::string(e.what()));
    }
}

} // namespace ui
