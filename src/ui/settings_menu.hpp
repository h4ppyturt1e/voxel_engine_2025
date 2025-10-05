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
    void renderAudioSettings();
    void renderUISettings();
    void renderControls();
    
    void saveSettings();
    void loadSettings();
    void applySettings();
    
    // Settings cache
    struct {
        bool vsync;
        int resolution_width;
        int resolution_height;
        std::string quality;
        float master_volume;
        float sfx_volume;
        float music_volume;
        std::string device;
        float mouse_sensitivity;
        std::string theme;
        float scale;
    } settings_;
    
    bool settings_changed_ = false;
};

} // namespace ui
