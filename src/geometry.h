#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <ostream>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class Vector4;

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
	inline Vec3<T> operator -() const { return Vec3<T>(-x, -y, -z); }
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

	Vector4<T> ToPoint() const;
	Vector4<T> ToDirection() const;
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

template
<typename T>
class Vector4
{
public:
	Vector4 ()
		: m_x(0.f), m_y(0.f), m_z(0.f), m_w(0.f) {}

	Vector4(T x, T y, T z, T w)
		: m_x(x), m_y(y), m_z(z), m_w(w) {}

	Vector4(const T arr[4])
		: m_x(arr[0]), m_y(arr[1]), m_z(arr[2]), m_w(arr[3]) {}

	Vector4(Vec3<T> vec3, T w)
		: m_x(vec3.x), m_y(vec3.y), m_z(vec3.z), m_w(w) {}

	T dot(const Vector4& vec)
	{
		return m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z + m_w * vec.m_w;
	}

	T x()  const { return m_x; }
	T y()  const { return m_y; }
	T z()  const { return m_z; }
	T w()  const { return m_w; }

	T* getRaw() { return &m_raw[0]; }

	Vector4& operator*(float scaler)
	{
		m_x *= scaler;
		m_y *= scaler;
		m_z *= scaler;
		m_w *= scaler;

		return *this;
	}

	Vec3<T> FromHomogeneous()
	{
		float w = m_w != 0.0f ? m_w : 1.0;
		return Vec3<T> { m_x / w, m_y / w, m_z / w };
	}

	friend std::ostream& operator <<(std::ostream& os, const Vector4& vec);

private:
	union
	{
		struct { T m_x, m_y, m_z, m_w; };
		struct { T m_r, m_g, m_b, m_a; };
		T m_raw[4];
	};
};

template <typename T>
inline std::ostream& operator <<(std::ostream& os, const Vector4<T>& vec)
{
	os << '(' << vec.m_x << ", " << vec.m_y << ", " << vec.m_z << ", " << vec.m_w << ')';

	return os;
}

template <typename T>
Vector4<T> Vec3<T>::ToPoint() const
{
	return Vector4<T>(x, y, z, 1.0f);
}

template <typename T>
Vector4<T> Vec3<T>::ToDirection() const
{
	return Vector4<T>(x, y, z, 0.0f);
}


template
<typename T>
class Matrix4x4
{
public:
	static constexpr size_t type_size = sizeof(T);
	static constexpr size_t row_size = 4 * type_size;
	static constexpr size_t mat_size = 4 * row_size;

	Matrix4x4()
	{
		memset(raw, T{}, 16 * sizeof(T));
	}

	Matrix4x4(const Matrix4x4& mat)
		: Matrix4x4(mat.raw) {}

	Matrix4x4(const T raw_[16])
	{
		memcpy(raw, raw_, mat_size);
	}

	Matrix4x4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
	{
		rawMat[0][0] = m00; rawMat[0][1] = m01; rawMat[0][2] = m02; rawMat[0][3] = m03;
		rawMat[1][0] = m10; rawMat[1][1] = m11; rawMat[1][2] = m12; rawMat[1][3] = m13;
		rawMat[2][0] = m20; rawMat[2][1] = m21; rawMat[2][2] = m22; rawMat[2][3] = m23;
		rawMat[3][0] = m30; rawMat[3][1] = m31; rawMat[3][2] = m32; rawMat[3][3] = m33;
	}

	Matrix4x4(Vector4<T> row0, Vector4<T> row1, Vector4<T> row2, Vector4<T> row3)
	{
		memcpy(rawMat[0], row0.getRaw(), row_size);
		memcpy(rawMat[1], row1.getRaw(), row_size);
		memcpy(rawMat[2], row2.getRaw(), row_size);
		memcpy(rawMat[3], row3.getRaw(), row_size);
	}

	void SetIdentity();

	Matrix4x4 operator*(float scaler)
	{
		Matrix4x4 mat {*this};
		mat *= scaler;
		return mat;
	}

	void operator*=(float scaler)
	{
		for (int i{ 0 }; i < 16; i++)
			raw[i] *= scaler;
	}

	Matrix4x4 operator*(const Matrix4x4& mat)
	{
		return Matrix4x4
		{
			Vector4<T>{ rawMat[0] }.dot(mat.GetColumn(0)),
			Vector4<T>{ rawMat[0] }.dot(mat.GetColumn(1)),
			Vector4<T>{ rawMat[0] }.dot(mat.GetColumn(2)),
			Vector4<T>{ rawMat[0] }.dot(mat.GetColumn(3)),
					    
			Vector4<T>{ rawMat[1] }.dot(mat.GetColumn(0)),
			Vector4<T>{ rawMat[1] }.dot(mat.GetColumn(1)),
			Vector4<T>{ rawMat[1] }.dot(mat.GetColumn(2)),
			Vector4<T>{ rawMat[1] }.dot(mat.GetColumn(3)),
 
			Vector4<T>{ rawMat[2] }.dot(mat.GetColumn(0)),
			Vector4<T>{ rawMat[2] }.dot(mat.GetColumn(1)),
			Vector4<T>{ rawMat[2] }.dot(mat.GetColumn(2)),
			Vector4<T>{ rawMat[3] }.dot(mat.GetColumn(3)),
					    
			Vector4<T>{ rawMat[3] }.dot(mat.GetColumn(0)),
			Vector4<T>{ rawMat[3] }.dot(mat.GetColumn(1)),
			Vector4<T>{ rawMat[3] }.dot(mat.GetColumn(2)),
			Vector4<T>{ rawMat[3] }.dot(mat.GetColumn(3)),

			// do I need to rawdog it? or will the generated instructions be the same
			//rawMat[0][0] * mat.rawMat[0][0] + rawMat[0][1] * mat.rawMat[1][0] + rawMat[0][2] * mat.rawMat[2][0] + rawMat[0][3] * mat.rawMat[3][0],
			//rawMat[1][0] * mat.rawMat[0][0] + rawMat[1][1] * mat.rawMat[1][0] + rawMat[1][2] * mat.rawMat[2][0] + rawMat[1][3] * mat.rawMat[3][0],
			//rawMat[2][0] * mat.rawMat[0][0] + rawMat[2][1] * mat.rawMat[1][0] + rawMat[2][2] * mat.rawMat[2][0] + rawMat[2][3] * mat.rawMat[3][0],
			//rawMat[3][0] * mat.rawMat[0][0] + rawMat[3][1] * mat.rawMat[1][0] + rawMat[3][2] * mat.rawMat[2][0] + rawMat[3][3] * mat.rawMat[3][0],
		};
	}

	Vector4<T> operator*(const Vector4<T>& vec)
	{
		return Vector4<T>
		{
			Vector4<T>{ rawMat[0] }.dot(vec),
			Vector4<T>{ rawMat[1] }.dot(vec),
			Vector4<T>{ rawMat[2] }.dot(vec),
			Vector4<T>{ rawMat[3] }.dot(vec)
		};
	}

	T* operator[](int row)
	{
		return rawMat[row];
	}

	void SetRow(int rowIdx, const Vector4<T>& row)
	{
		memcpy(rawMat[rowIdx], row.m_raw, 4 * sizeof(T));
	}

	void SetRow(int rowIdx, const Vec3<T>& row)
	{
		memcpy(rawMat[rowIdx], row.raw, 3 * sizeof(T));
	}


	Vector4<T> GetColumn(int columnIdx) const
	{
		return Vector4<T> { rawMat[0][columnIdx], rawMat[1][columnIdx], rawMat[2][columnIdx], rawMat[3][columnIdx] };
	}

	void SetColumn(int columnIdx, const Vector4<T>& column)
	{
		rawMat[0][columnIdx] = column.x();
		rawMat[1][columnIdx] = column.y();
		rawMat[2][columnIdx] = column.z();
		rawMat[3][columnIdx] = column.w();
	}

	void SetElement(int row, int column, T value)
	{
		rawMat[row][column] = value;
	}

	union
	{
	T raw[4 * 4];
	T rawMat[4][4]; // [row][column]
	};
};

template <typename T>
void Matrix4x4<T>::SetIdentity()
{
	rawMat[0][0] = 1.f;
	rawMat[1][1] = 1.f;
	rawMat[2][2] = 1.f;
	rawMat[3][3] = 1.f;
}

using Vec4f = Vector4<float>;
using Mat4 = Matrix4x4<float>;

#endif //__GEOMETRY_H__
