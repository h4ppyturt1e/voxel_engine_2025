#include "overlay.hpp"
#include "ui_manager.hpp"

namespace ui {

Overlay::Overlay(OverlayType type, const std::string& name, bool modal)
    : type_(type), name_(name), modal_(modal) {
}

void Overlay::triggerShowEvent() {
    if (on_show_) {
        on_show_();
    }
}

void Overlay::triggerHideEvent() {
    if (on_hide_) {
        on_hide_();
    }
}

} // namespace ui
