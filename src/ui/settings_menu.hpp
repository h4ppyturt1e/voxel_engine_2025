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
		bool fullscreen;
    } settings_;
    
    // Temporary UI values (for sliders/inputs)
    struct {
        bool vsync;
        int resolution_width;
        int resolution_height;
        std::string quality;
        float mouse_sensitivity;
        float mouse_sensitivity_percent;
        std::string theme;
        float scale;
        bool crosshair_enabled;
        float crosshair_percent;
		bool fullscreen;
    } temp_settings_;
    
    bool settings_changed_ = false;

    // Resolution selection state
    int aspect_index_ = 0; // 0:16:9,1:16:10,2:4:3,3:21:9,4:32:9
    int res_index_ = 0;
};

} // namespace ui
