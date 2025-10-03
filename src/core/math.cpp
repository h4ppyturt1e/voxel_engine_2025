#include "math.hpp"

namespace core {

Mat4 Mat4::identity() {
	Mat4 r{};
	r.m[0]=1;r.m[5]=1;r.m[10]=1;r.m[15]=1;
	return r;
}

Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane) {
	Mat4 r{};
	float f = 1.0f / std::tan(fovRadians * 0.5f);
	r.m[0] = f / aspect;
	r.m[5] = f;
	r.m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
	r.m[11] = -1.0f;
	r.m[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
	return r;
}

Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
	Vec3 f{ center.x - eye.x, center.y - eye.y, center.z - eye.z };
	float fl = std::sqrt(f.x*f.x+f.y*f.y+f.z*f.z); f.x/=fl; f.y/=fl; f.z/=fl;
	Vec3 s{ f.y*up.z - f.z*up.y, f.z*up.x - f.x*up.z, f.x*up.y - f.y*up.x };
	float sl = std::sqrt(s.x*s.x+s.y*s.y+s.z*s.z); s.x/=sl; s.y/=sl; s.z/=sl;
	Vec3 u{ s.y*f.z - s.z*f.y, s.z*f.x - s.x*f.z, s.x*f.y - s.y*f.x };
	Mat4 r = Mat4::identity();
	r.m[0]=s.x; r.m[4]=s.y; r.m[8]=s.z;
	r.m[1]=u.x; r.m[5]=u.y; r.m[9]=u.z;
	r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;
	r.m[12]=-(s.x*eye.x + s.y*eye.y + s.z*eye.z);
	r.m[13]=-(u.x*eye.x + u.y*eye.y + u.z*eye.z);
	r.m[14]= f.x*eye.x + f.y*eye.y + f.z*eye.z;
	return r;
}

} // namespace core


