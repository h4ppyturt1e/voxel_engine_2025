#include "input.hpp"
#include <unordered_set>

namespace input {

namespace {
	std::unordered_set<int> g_down;
}

void InputSystem::setKey(int key, KeyState state) {
	if (state == KeyState::Down) g_down.insert(key);
	else g_down.erase(key);
}

bool InputSystem::isKeyDown(int key) const {
	return g_down.find(key) != g_down.end();
}

} // namespace input


