#include "theme.hpp"
#include "../config/ini_parser.hpp"

#ifdef VOXEL_WITH_GL
#include <imgui.h>
#endif

#include <sstream>
#include <algorithm>

namespace ui {

static bool parseHexColor(std::string s, float out[4]) {
    // Accept formats: #RRGGBB, #RRGGBBAA, RRGGBB, RRGGBBAA
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    if (!s.empty() && (s[0] == '#')) s.erase(s.begin());
    if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) s = s.substr(2);
    if (!(s.size() == 6 || s.size() == 8)) return false;
    auto hexToByte = [](const std::string& h) -> int {
        return std::stoi(h, nullptr, 16);
    };
    try {
        int r = hexToByte(s.substr(0,2));
        int g = hexToByte(s.substr(2,2));
        int b = hexToByte(s.substr(4,2));
        int a = (s.size()==8) ? hexToByte(s.substr(6,2)) : 255;
        out[0] = r / 255.0f; out[1] = g / 255.0f; out[2] = b / 255.0f; out[3] = a / 255.0f;
        return true;
    } catch (...) {
        return false;
    }
}

bool Theme::loadFromFile(const std::string& path, const std::string& themeName) {
    config::IniParser parser;
    if (!parser.parseFile(path)) return false;
    // Read UI-wide settings from [ui]
    auto baseSec = parser.section("ui");
    std::string tmp;
    auto get = [&](const char* k, std::string& dst){ auto it=baseSec.find(k); if(it!=baseSec.end()){ dst=it->second; return true;} return false; };
    if (get("style", tmp)) baseStyle_ = tmp; else baseStyle_ = themeName.empty()?"dark":themeName;
    if (get("font_scale", tmp)) fontScale_ = std::stof(tmp);
    if (get("alpha", tmp)) alpha_ = std::stof(tmp);
    if (get("window_rounding", tmp)) windowRounding_ = std::stof(tmp);
    if (get("frame_rounding", tmp)) frameRounding_ = std::stof(tmp);
    if (get("grab_rounding", tmp)) grabRounding_ = std::stof(tmp);
    // Collect color overrides for the theme-specific colors section: [theme.<name>]
    colorKVs_.clear();
    std::string tname = themeName.empty()?"dark":themeName;
    auto colors = parser.section(std::string("theme.") + tname);
    for (const auto& kv : colors) {
        colorKVs_.push_back({kv.first, kv.second});
    }
    // Fallback legacy sections if not found
    if (colorKVs_.empty()) {
        auto legacyNamed = parser.section(tname + ".colors");
        if (!legacyNamed.empty()) {
            for (const auto& kv : legacyNamed) colorKVs_.push_back({kv.first, kv.second});
        } else {
            auto legacy = parser.section("colors");
            for (const auto& kv : legacy) colorKVs_.push_back({kv.first, kv.second});
        }
    }
    return true;
}

void Theme::apply() const {
#ifdef VOXEL_WITH_GL
    if (baseStyle_ == "light") ImGui::StyleColorsLight();
    else ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    if (alpha_ >= 0.0f) s.Alpha = alpha_;
    if (windowRounding_ >= 0.0f) s.WindowRounding = windowRounding_;
    if (frameRounding_ >= 0.0f) s.FrameRounding = frameRounding_;
    if (grabRounding_ >= 0.0f) s.GrabRounding = grabRounding_;
    for (const auto& kv : colorKVs_) {
        float rgba[4];
        if (!parseHexColor(kv.second, rgba)) continue;
        // Map name to ImGuiCol index (subset support)
        int idx = -1;
        const std::string& n = kv.first;
        if (n == "Text") idx = ImGuiCol_Text;
        else if (n == "WindowBg") idx = ImGuiCol_WindowBg;
        else if (n == "FrameBg") idx = ImGuiCol_FrameBg;
        else if (n == "FrameBgHovered") idx = ImGuiCol_FrameBgHovered;
        else if (n == "FrameBgActive") idx = ImGuiCol_FrameBgActive;
        else if (n == "TitleBg") idx = ImGuiCol_TitleBg;
        else if (n == "TitleBgActive") idx = ImGuiCol_TitleBgActive;
        else if (n == "CheckMark") idx = ImGuiCol_CheckMark;
        else if (n == "Button") idx = ImGuiCol_Button;
        else if (n == "ButtonHovered") idx = ImGuiCol_ButtonHovered;
        else if (n == "ButtonActive") idx = ImGuiCol_ButtonActive;
        if (idx >= 0) s.Colors[idx] = ImVec4(rgba[0], rgba[1], rgba[2], rgba[3]);
    }
    // Font global scale is in IO
    ImGui::GetIO().FontGlobalScale = fontScale_;
#endif
}

} // namespace ui


