#pragma once

#include <string>
#include <vector>

namespace ui {

class Theme {
public:
    // Load a named theme from file. Expects sections like [ui] and [theme.<name>]
    // Falls back to legacy [imgui] and [colors] if not present
    bool loadFromFile(const std::string& path, const std::string& themeName);
    void apply() const;

private:
    std::string baseStyle_ {"dark"};
    float fontScale_ {1.0f};
    std::string fontPath_ {};
    float fontSize_ {16.0f};
    bool fontEnabled_ {true};
    float alpha_ {1.0f};
    float windowRounding_ {-1.0f};
    float frameRounding_ {-1.0f};
    float grabRounding_ {-1.0f};
    // Colors stored as key -> hex string; parsed on apply
    std::vector<std::pair<std::string, std::string>> colorKVs_;
};

} // namespace ui

