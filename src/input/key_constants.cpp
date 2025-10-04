#include "key_constants.hpp"
#include <unordered_map>

namespace input {

int keyNameToCode(const std::string& keyName) {
    static const std::unordered_map<std::string, int> keyMap = {
        // Letters
        {"A", 65}, {"B", 66}, {"C", 67}, {"D", 68}, {"E", 69}, {"F", 70},
        {"G", 71}, {"H", 72}, {"I", 73}, {"J", 74}, {"K", 75}, {"L", 76},
        {"M", 77}, {"N", 78}, {"O", 79}, {"P", 80}, {"Q", 81}, {"R", 82},
        {"S", 83}, {"T", 84}, {"U", 85}, {"V", 86}, {"W", 87}, {"X", 88},
        {"Y", 89}, {"Z", 90},
        
        // Numbers
        {"0", 48}, {"1", 49}, {"2", 50}, {"3", 51}, {"4", 52},
        {"5", 53}, {"6", 54}, {"7", 55}, {"8", 56}, {"9", 57},
        
        // Special keys
        {"SPACE", 32}, {"ENTER", 257}, {"ESCAPE", 256},
        {"SHIFT", 340}, {"CTRL", 341}, {"ALT", 342},
        
        // Arrow keys
        {"UP", 265}, {"DOWN", 264}, {"LEFT", 263}, {"RIGHT", 262},
        
        // Function keys
        {"F1", 290}, {"F2", 291}, {"F3", 292}, {"F4", 293}, {"F5", 294},
        {"F6", 295}, {"F7", 296}, {"F8", 297}, {"F9", 298}, {"F10", 299},
        {"F11", 300}, {"F12", 301},
        
        // Mouse buttons
        {"MOUSE_LEFT", 0}, {"MOUSE_RIGHT", 1}, {"MOUSE_MIDDLE", 2}
    };
    
    auto it = keyMap.find(keyName);
    return (it != keyMap.end()) ? it->second : -1;
}

std::string keyCodeToName(int keyCode) {
    static const std::unordered_map<int, std::string> codeMap = {
        // Letters
        {65, "A"}, {66, "B"}, {67, "C"}, {68, "D"}, {69, "E"}, {70, "F"},
        {71, "G"}, {72, "H"}, {73, "I"}, {74, "J"}, {75, "K"}, {76, "L"},
        {77, "M"}, {78, "N"}, {79, "O"}, {80, "P"}, {81, "Q"}, {82, "R"},
        {83, "S"}, {84, "T"}, {85, "U"}, {86, "V"}, {87, "W"}, {88, "X"},
        {89, "Y"}, {90, "Z"},
        
        // Numbers
        {48, "0"}, {49, "1"}, {50, "2"}, {51, "3"}, {52, "4"},
        {53, "5"}, {54, "6"}, {55, "7"}, {56, "8"}, {57, "9"},
        
        // Special keys
        {32, "SPACE"}, {257, "ENTER"}, {256, "ESCAPE"},
        {340, "SHIFT"}, {341, "CTRL"}, {342, "ALT"},
        
        // Arrow keys
        {265, "UP"}, {264, "DOWN"}, {263, "LEFT"}, {262, "RIGHT"},
        
        // Function keys
        {290, "F1"}, {291, "F2"}, {292, "F3"}, {293, "F4"}, {294, "F5"},
        {295, "F6"}, {296, "F7"}, {297, "F8"}, {298, "F9"}, {299, "F10"},
        {300, "F11"}, {301, "F12"},
        
        // Mouse buttons
        {0, "MOUSE_LEFT"}, {1, "MOUSE_RIGHT"}, {2, "MOUSE_MIDDLE"}
    };
    
    auto it = codeMap.find(keyCode);
    return (it != codeMap.end()) ? it->second : "";
}

} // namespace input
