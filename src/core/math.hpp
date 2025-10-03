#pragma once

#include <cmath>

namespace core {

struct Vec3 {
	float x{0}, y{0}, z{0};
};

struct Mat4 {
	float m[16];
	static Mat4 identity();
};

Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane);
Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);

} // namespace core


