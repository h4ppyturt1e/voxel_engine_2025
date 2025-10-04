#pragma once

#include <string>

namespace input {

// Key name to GLFW key code mapping
// This allows us to use human-readable names in input.ini
int keyNameToCode(const std::string& keyName);
std::string keyCodeToName(int keyCode);

} // namespace input
