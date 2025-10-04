#pragma once

namespace input {

enum class KeyState { Up, Down };

class InputSystem {
public:
	void setKey(int key, KeyState state);
	bool isKeyDown(int key) const;
};

} // namespace input


