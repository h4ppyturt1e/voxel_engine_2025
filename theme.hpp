#pragma once

#include <string>
#include <vector>

namespace ui {

// Loads ImGui style parameters from an ini-like file and applies them.
// Supported keys (section [imgui]):
//  style=dark|light
//  font_scale=1.0
//  alpha=1.0
//  window_rounding=..., frame_rounding=..., grab_rounding=...
//  colors.<ImGuiCol_*> = r,g,b,a  (e.g., colors.WindowBg=0.1,0.1,0.1,1.0)
class Theme {
public:
    // Load a named theme from file. Expects sections like [dark] and [dark.colors].
    // If the named sections are missing, falls back to legacy [imgui] and [colors].
    bool loadFromFile(const std::string& path, const std::string& themeName);
    void apply() const;

private:
    std::string baseStyle_ {"dark"};
    float fontScale_ {1.0f};
    std::string fontPath_ {};
    float fontSize_ {16.0f};
    float alpha_ {1.0f};
    float windowRounding_ {-1.0f};
    float frameRounding_ {-1.0f};
    float grabRounding_ {-1.0f};
    // Colors are stored as full key -> rgba string, parsed at apply
    std::vector<std::pair<std::string, std::string>> colorKVs_;
};

} // namespace ui


