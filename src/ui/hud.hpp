#pragma once

#include "overlay.hpp"

namespace ui {

class HUD : public Overlay {
public:
    HUD();
    ~HUD() override = default;
    
    void render() override;
    void update(float deltaTime) override;
    
private:
    void renderCrosshair();
    void renderFPS();
    void renderDebugInfo();
    
    float fps_ = 0.0f;
    int frame_count_ = 0;
    float fps_timer_ = 0.0f;
    bool show_debug_ = false;
};

} // namespace ui
