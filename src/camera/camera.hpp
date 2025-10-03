#pragma once

namespace camera {

class Camera {
public:
	void setPerspective(float fovRadians, float aspect, float nearPlane, float farPlane);
};

} // namespace camera


