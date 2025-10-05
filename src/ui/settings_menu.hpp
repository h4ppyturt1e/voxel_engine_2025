#pragma once

#include "overlay.hpp"

namespace ui {

class SettingsMenu : public Overlay {
public:
    SettingsMenu();
    ~SettingsMenu() override = default;
    
    void render() override;
    void update(float deltaTime) override;
    
private:
    void renderGraphicsSettings();
    void renderUISettings();
    void renderControls();
    
    void saveSettings();
    void loadSettings();
    void applySettings();
    void resetTempSettings();
    
    // Settings cache (applied values)
    struct {
        bool vsync;
        int resolution_width;
        int resolution_height;
        std::string quality;
        float mouse_sensitivity;
        std::string theme;
        float scale;
        bool crosshair_enabled;
        float crosshair_percent;
    } settings_;
    
    // Temporary UI values (for sliders/inputs)
    struct {
        bool vsync;
        int resolution_width;
        int resolution_height;
        std::string quality;
        float mouse_sensitivity;
        std::string theme;
        float scale;
        bool crosshair_enabled;
        float crosshair_percent;
    } temp_settings_;
    
    bool settings_changed_ = false;
};

} // namespace ui
