#pragma once

#include "overlay.hpp"

namespace ui {

class KeyBindingsMenu : public Overlay {
public:
    KeyBindingsMenu();
    ~KeyBindingsMenu() override = default;
    
    void render() override;
    void update(float deltaTime) override;
    
private:
    void renderKeyBinding(const std::string& action, const std::string& currentKey);
    void saveKeyBindings();
    void loadKeyBindings();
    
    bool is_remapping_ = false;
    std::string remapping_action_;
};

} // namespace ui
