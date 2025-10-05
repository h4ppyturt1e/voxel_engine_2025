#include "ini_parser.hpp"
#include <fstream>

namespace config {

void IniParser::trim(std::string& s) {
    while (!s.empty() && (s.front()==' '||s.front()=='\t')) s.erase(s.begin());
    while (!s.empty() && (s.back()==' '||s.back()=='\t' || s.back()=='\r' || s.back()=='\n')) s.pop_back();
}

bool IniParser::parseFile(const std::string& path) {
    entries_.clear();
    std::ifstream in(path);
    if (!in) return false;
    std::string line;
    std::string currentSection;
    while (std::getline(in, line)) {
        trim(line);
        if (line.empty() || line[0]=='#' || line[0]==';') continue;
        if (line.front()=='[' && line.back()==']') {
            currentSection = line.substr(1, line.size()-2);
            trim(currentSection);
            continue;
        }
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq+1);
        trim(key); trim(val);
        std::string fullKey = currentSection.empty() ? key : (currentSection + "." + key);
        entries_.emplace_back(fullKey, val);
    }
    return true;
}

std::unordered_map<std::string, std::string> IniParser::section(const std::string& sectionName) const {
    std::unordered_map<std::string, std::string> out;
    const std::string prefix = sectionName + ".";
    for (const auto& [k, v] : entries_) {
        if (k.rfind(prefix, 0) == 0) {
            out.emplace(k.substr(prefix.size()), v);
        }
    }
    return out;
}

bool IniParser::tryGet(const std::string& fullKey, std::string& out) const {
    for (const auto& [k, v] : entries_) {
        if (k == fullKey) { out = v; return true; }
    }
    return false;
}

} // namespace config


