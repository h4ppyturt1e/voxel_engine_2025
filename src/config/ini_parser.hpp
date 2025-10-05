#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace config {

// Minimal, generic INI parser supporting:
// - Sections in [section]
// - key=value pairs
// - Comments starting with # or ;
// - Trims whitespace; empty lines ignored
// Produces full keys as "section.key" for sectioned entries, or just "key" for root.
class IniParser {
public:
    bool parseFile(const std::string& path);

    // Return all entries as vector of (fullKey, value)
    const std::vector<std::pair<std::string, std::string>>& entries() const { return entries_; }

    // Get map of key->value for a given section
    std::unordered_map<std::string, std::string> section(const std::string& sectionName) const;

    // Get value by full key (e.g. "graphics.resolution_width")
    bool tryGet(const std::string& fullKey, std::string& out) const;

private:
    static void trim(std::string& s);
    std::vector<std::pair<std::string, std::string>> entries_;
};

} // namespace config


