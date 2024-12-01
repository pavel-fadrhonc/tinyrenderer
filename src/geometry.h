#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <ostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> struct Vec2 {
	union {
		struct {T u, v;};
		struct {T x, y;};
		T raw[2];
	};
	constexpr Vec2() : u(0), v(0) {}
	constexpr Vec2(T _u, T _v) : u(_u),v(_v) {}
	inline Vec2<T> operator +(const Vec2<T> &V) const { return Vec2<T>(u+V.u, v+V.v); }
	inline Vec2<T> operator -(const Vec2<T> &V) const { return Vec2<T>(u-V.u, v-V.v); }
	inline Vec2<T> operator *(float f)          const { return Vec2<T>(u*f, v*f); }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <typename T> struct Vec3 {
	union {
		struct {T x, y, z;};
		struct { T ivert, iuv, inorm; };
		T raw[3];
	};
	constexpr Vec3() : x(0), y(0), z(0) {}
	constexpr Vec3(T _x, T _y, T _z) : x(_x),y(_y),z(_z) {}
	constexpr Vec3(const Vec2<T>& vec2) : x(vec2.x), y(vec2.y), z(0) {}
	inline Vec3<T> operator ^(const Vec3<T> &v) const { return Vec3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<T> operator +(const Vec3<T> &v) const { return Vec3<T>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<T> operator -(const Vec3<T> &v) const { return Vec3<T>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<T> operator *(float f)          const { return Vec3<T>(x*f, y*f, z*f); }
	inline T       operator *(const Vec3<T> &v) const { return x*v.x + y*v.y + z*v.z; }
	float magnitude () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<T> & normalize(T l=1) { *this = (*this)*(l/magnitude()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<T>& v);

	Vec3 cross(const Vec3<T>& v) const
	{
		return Vec3{
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		};
	}

	T dot(const Vec3<T>& v) const
	{
		return v.x * x + v.y * y + v.z * z;
	}

	float sqrtMagnitude() { return this->dot(*this); }

	Vec2<T> ToVec2() { return Vec2<T> {x, y}; };
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

inline float TriangleArea(const Vec3i& v1, const Vec3i& v2, const Vec3i& v3)
{
	return (v2 - v1).cross(v3 - v1).magnitude() * 0.5f;
}

constexpr Vec3f VForward { 0, 0, 1.f };
constexpr Vec3f VBack = Vec3f{ 0, 0, -1.f };
constexpr Vec3f VRight = Vec3f{ 1, 0, 0.f };
constexpr Vec3f VLight = Vec3f{ -1, 0, 0.f };
constexpr Vec3f VUp = Vec3f{0, 1.0f, 0.f};
constexpr Vec3f VDown = Vec3f{0, -1.0f, 0.f};

#endif //__GEOMETRY_H__
