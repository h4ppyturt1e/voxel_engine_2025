#pragma once

#include <string>
#include <functional>

namespace ui {

enum class OverlayType;

class Overlay {
public:
    Overlay(OverlayType type, const std::string& name, bool modal = false);
    virtual ~Overlay() = default;
    
    // Core interface
    virtual void render() = 0;
    virtual void update(float deltaTime) {}
    
    // State management
    void show() { visible_ = true; }
    void hide() { visible_ = false; }
    void toggle() { visible_ = !visible_; }
    bool isVisible() const { return visible_; }
    
    // Properties
    OverlayType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    bool isModal() const { return modal_; }
    
    // Events
    using EventCallback = std::function<void()>;
    void setOnShowCallback(EventCallback callback) { on_show_ = callback; }
    void setOnHideCallback(EventCallback callback) { on_hide_ = callback; }
    
protected:
    void triggerShowEvent();
    void triggerHideEvent();
    
private:
    OverlayType type_;
    std::string name_;
    bool modal_;
    bool visible_ = false;
    
    EventCallback on_show_;
    EventCallback on_hide_;
};

} // namespace ui
