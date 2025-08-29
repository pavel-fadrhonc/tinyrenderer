#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <algorithm>
#include <array>
#include <assert.h>
#include <cmath>
#include <ostream>

#include "types.h"

namespace sor
{
	
	template<typename T>
	class Vector4;

	template <typename T> struct Vec2 {
		union {
			struct { T u, v; };
			struct { T x, y; };
			T raw[2];
		};
		constexpr Vec2() : u(0), v(0) {}
		constexpr Vec2(T _u, T _v) : u(_u), v(_v) {}
		inline Vec2<T> operator +(const Vec2<T>& V) const { return Vec2<T>(u + V.u, v + V.v); }
		inline Vec2<T> operator -(const Vec2<T>& V) const { return Vec2<T>(u - V.u, v - V.v); }
		inline Vec2<T> operator *(float f)          const { return Vec2<T>(u * f, v * f); }
		template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
	};

	template <typename T> struct Vec3 {
		union {
			struct { T x, y, z; };
			struct { T ivert, iuv, inorm; };
			T raw[3];
		};
		constexpr Vec3() : x(0), y(0), z(0) {}
		constexpr Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
		constexpr Vec3(const Vec2<T>& vec2) : x(vec2.x), y(vec2.y), z(0) {}
		constexpr Vec3(const std::array<T, 3>& arr) : x(arr[0]), y(arr[1]), z(arr[2]) {}
		inline Vec3<T> operator ^(const Vec3<T>& v) const { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
		inline Vec3<T> operator +(const Vec3<T>& v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
		inline Vec3<T> operator -(const Vec3<T>& v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
		inline Vec3<T> operator -() const { return Vec3<T>(-x, -y, -z); }
		inline Vec3<T> operator -(const float v) { return Vec3{ x - v, y - v, z - v }; }
		inline Vec3<T> operator *(float f)          const { return Vec3<T>(x * f, y * f, z * f); }
		inline T       operator *(const Vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }
		inline Vec3 operator / (const float f) const { return { x / f, y / f, z / f }; }
		float magnitude() const { return std::sqrt(x * x + y * y + z * z); }
		Vec3<T>& normalize(T l = 1) { *this = (*this) * (l / magnitude()); return *this; }
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

		friend std::ostream& operator<<(std::ostream& os, const Vec3& v)
		{
			os << "(x: " << v.x << ", y: " << v.y << ", z: " << v.z << ")";

			return os;
		}
	};

	typedef Vec2<float> Vec2f;
	typedef Vec2<int>   Vec2i;
	typedef Vec3<float> Vec3f;
	typedef Vec3<double> Vec3d;
	typedef Vec3<int>   Vec3i;
	typedef Vec3<u64>	Vec3u64;

	template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
		s << "(" << v.x << ", " << v.y << ")";
		return s;
	}

	template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
		s << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return s;
	}

	inline float TriangleArea(const Vec3i& v1, const Vec3i& v2, const Vec3i& v3)
	{
		return (v2 - v1).cross(v3 - v1).magnitude() * 0.5f;
	}

	constexpr Vec3f VForward{ 0, 0, 1.f };
	constexpr Vec3f VBack = Vec3f{ 0, 0, -1.f };
	constexpr Vec3f VRight = Vec3f{ 1, 0, 0.f };
	constexpr Vec3f VLight = Vec3f{ -1, 0, 0.f };
	constexpr Vec3f VUp = Vec3f{ 0, 1.0f, 0.f };
	constexpr Vec3f VDown = Vec3f{ 0, -1.0f, 0.f };

	template
		<typename T>
		class Vector4
	{
	public:
		Vector4()
			: m_x(0.f), m_y(0.f), m_z(0.f), m_w(0.f) {
		}

		Vector4(T x, T y, T z, T w)
			: m_x(x), m_y(y), m_z(z), m_w(w) {
		}

		Vector4(const T arr[4])
			: m_x(arr[0]), m_y(arr[1]), m_z(arr[2]), m_w(arr[3]) {
		}

		Vector4(Vec3<T> vec3, T w)
			: m_x(vec3.x), m_y(vec3.y), m_z(vec3.z), m_w(w) {
		}

		T dot(const Vector4& vec)
		{
			return m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z + m_w * vec.m_w;
		}

		T x()  const { return m_x; }
		T y()  const { return m_y; }
		T z()  const { return m_z; }
		T w()  const { return m_w; }

		// T* getRaw() { return &m_raw[0]; }
		// const T* getRaw() const { return &m_raw[0]; }
		const RawArrVariableSize<T, 4>& getRaw() const { return m_raw; }
		RawArrVariableSize<T, 4>& getRaw() { return m_raw; }

		Vector4 operator*(float scalar) const
		{
			return { m_x * scalar, m_y * scalar, m_z * scalar, m_w * scalar };
		}

		Vector4& operator*=(float scaler)
		{
			m_x *= scaler;
			m_y *= scaler;
			m_z *= scaler;
			m_w *= scaler;

			return *this;
		}

		Vector4<T> operator-(const Vector4<T>& v)
		{
			return { m_x - v.x(), m_y - v.y(), m_z - v.z(), m_w - v.w() };
		}


		Vector4<T> operator+(const Vector4<T>& v)
		{
			return { m_x + v.x(), m_y + v.y(), m_z + v.z(), m_w + v.w() };
		}

		Vector4 operator*(Vector4 v) const
		{
			return { m_x * v.x(), m_y * v.y(), m_z * v.z(), m_w * v.w() };
		}

		Vec3<T> FromHomogeneous() const
		{
			float w = m_w != 0.0f ? m_w : 1.0;
			return Vec3<T> { m_x / w, m_y / w, m_z / w };
		}

		Vec3<T> ToVec3()
		{
			return Vec3<T> {m_x, m_y, m_z};
		}

		void Clamp01()
		{
			m_x = std::clamp(m_x, 0.0f, 1.0f);
			m_y = std::clamp(m_y, 0.0f, 1.0f);
			m_z = std::clamp(m_z, 0.0f, 1.0f);
			m_w = std::clamp(m_w, 0.0f, 1.0f);
		}

		//friend std::ostream& operator <<(std::ostream& os, const Vector4& vec);
		inline friend std::ostream& operator <<(std::ostream& os, const Vector4<T>& vec)
		{
			os << '(' << vec.m_x << ", " << vec.m_y << ", " << vec.m_z << ", " << vec.m_w << ')';

			return os;
		}

	private:
		union
		{
			struct { T m_x, m_y, m_z, m_w; };
			struct { T m_r, m_g, m_b, m_a; };
			RawArr4<T> m_raw;
		};
	};

	// template <typename T>
	// inline std::ostream& operator <<(std::ostream& os, const Vector4<T>& vec)
	// {
	// 	os << '(' << vec.m_x << ", " << vec.m_y << ", " << vec.m_z << ", " << vec.m_w << ')';
	//
	// 	return os;
	// }

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

	using Vec4f = Vector4<float>;


	// template <float>
	// inline std::ostream& operator <<(std::ostream& os, const Vec4f& vec)
	// {
	// 	os << '(' << vec.m_x << ", " << vec.m_y << ", " << vec.m_z << ", " << vec.m_w << ')';
	//
	// 	return os;
	// }

	template
		<typename T, u8 rows, u8 columns>
		class MatrixGeneric
	{
	public:
		static constexpr u8 row_count = rows;
		static constexpr u8 column_count = columns;
		static constexpr size_t element_count = rows * columns;

		static constexpr size_t type_size = sizeof(T);
		static constexpr size_t row_size = row_count * type_size;
		static constexpr size_t mat_size = column_count * row_size;

		using ColumnArrType = std::array<T, row_count>;

		MatrixGeneric()
		{
			memset(raw, T{}, mat_size);
		}

		MatrixGeneric(const MatrixGeneric& mat)
			: MatrixGeneric(mat.raw) {
		}

		MatrixGeneric(const T raw_[element_count])
		{
			memcpy(raw, raw_, mat_size);
		}

		MatrixGeneric(const std::array<T, element_count>& arr)
		{
			std::copy(arr.begin(), arr.end(), raw);
		}

		void SetIdentity()
		{
			static_assert(row_count == column_count, "Identity is only defined for square matrices");

			for (int i = 0; i < row_count; i++)
				rawMat[i][i] = static_cast<T>(1);
		}

		MatrixGeneric operator*(float scaler)
		{
			MatrixGeneric mat{ *this };
			mat *= scaler;
			return mat;
		}

		void operator*=(float scaler)
		{
			for (int i{ 0 }; i < element_count; i++)
				raw[i] *= scaler;
		}

		MatrixGeneric operator*(const MatrixGeneric& mat)
		{
			static_assert(row_count == column_count);

			MatrixGeneric mat_;
			for (int i = 0; i < row_count; i++)
			{
				for (int j = 0; j < row_count; j++)
				{
					T matElement = static_cast<T>(0);
					for (int k = 0; k < row_count; k++)
						matElement += rawMat[i][k] * mat.rawMat[k][j];

					mat_.rawMat[i][j] = matElement;
				}
			}

			return mat_;
		}

		ColumnArrType operator*(const ColumnArrType& arr)
		{
			ColumnArrType retArr;

			for (int i = 0; i < row_count; i++)
				for (int j = 0; j < column_count; j++)
					retArr[i] += rawMat[i][j] * arr[j];

			return retArr;
		}

		T* operator[](int row)
		{
			return rawMat[row];
		}

		void SetRow(int rowIdx, const RawArrVariableSize<T, row_count>& row)
		{
			memcpy(rawMat[rowIdx], row, row_count * sizeof(T));
		}

		void SetRow(int rowIdx, T* row)
		{
			memcpy(rawMat[rowIdx], row, row_count * sizeof(T));
		}

		void SetRow(int rowIdx, std::array<T, row_count> arr)
		{
			SetRow(rowIdx, arr.data());
		}

		// set pitch (rotation around x axis)
		void SetPitch(float angle)
		{
			static_assert(row_count >= 3 && column_count >= 3, "Matrix has to be at least 3x3 in order for pitch to be applied.");
			// 1	0			0	
			// 0	cos(alpha)	-sin(alpha)
			// 0	sin(alpha)	cos(alpha)
			float cosA = std::cosf(angle);
			float sinA = std::sinf(angle);

			SetRow(0, { 1.f, 0.f, 0.f });
			SetRow(1, { 0.f, cosA, -sinA });
			SetRow(2, { 0.f, sinA, cosA });
		}

		// set yaw (rotation around y axis)
		void SetYaw(float angle)
		{
			static_assert(row_count >= 3 && column_count >= 3, "Matrix has to be at least 3x3 in order for pitch to be applied.");
			// cos(alpha)	0	sin(alpha)
			// 0			1	0
			// -sin(alpha)	0	cos(alpha)
			float cosA = std::cosf(angle);
			float sinA = std::sinf(angle);

			SetRow(0, { cosA, 0.f, sinA });
			SetRow(1, { 0.f, 1.f, 0.f });
			SetRow(2, { -sinA, 0.f, cosA });
		}

		void SetRoll(float angle)
		{
			static_assert(row_count >= 2 && column_count >= 2, "Matrix has to be at least 2x2 in order for pitch to be applied.");
			// cos(alpha)	-sin(alpha)	0
			// sin(alpha)	cos(alpha)	0
			//	0			0			1
			float cosA = std::cosf(angle);
			float sinA = std::sinf(angle);

			SetRow(0, { cosA, -sinA, 0.f });
			SetRow(1, { sinA, cosA, 0.f });
			SetRow(2, { 0.f, 0.f, 1.f });
		}

		float GetPitch()
		{
			// sin(alpha) = sin(forward.y)
			return acos(GetElement(1, 2));
		}

		float GetYaw()
		{
			// tan(alpha) = forward.x / forward.z
			return atan2(GetElement(0, 2), GetElement(2, 2));
		}

		float GetRoll()
		{
			// tan(alpha) = right.y / right.x
			return atan2(GetElement(1, 0), GetElement(0, 0));
		}

		std::array<T, row_count> GetColumn(int columnIdx) const
		{
			assert(columnIdx < column_count);

			std::array<T, row_count> column;
			for (int i = 0; i < row_count; i++)
			{
				column[i] = rawMat[i][columnIdx];
			}

			return column;
		}

		void SetColumn(int columnIdx, const std::array<T, row_count>& arr)
		{
			assert(columnIdx < column_count);

			for (int i = 0; i < row_count; i++)
			{
				rawMat[i][columnIdx] = arr[i];
			}
		}

		void SetElement(int row, int column, T value)
		{
			assert(row < row_count && column < column_count);

			rawMat[row][column] = value;
		}

		MatrixGeneric<T, column_count, row_count> GetTranspose() const
		{
			MatrixGeneric<T, column_count, row_count> transposed;
			for (int i = 0; i < row_count; i++)
				for (int j = 0; j < column_count; j++)
					transposed[j][i] = rawMat[i][j];

			return transposed;
		}

		MatrixGeneric<T, row_count - 1, column_count - 1> GetCofactor(int x, int y) const
		{
			assert(row_count == column_count);

			MatrixGeneric<T, row_count - 1, column_count - 1> cofMat{};
			size_t i = 0, j = 0; // Track rows and columns of cofMat

			// Loop through all elements of the matrix
			for (size_t row = 0; row < row_count; row++) {
				for (size_t col = 0; col < column_count; col++) {
					// Skip the current row and column
					if (row != x && col != y) {
						cofMat.SetElement(i, j++, GetElement(row, col));

						// Move to the next row in temp if the column is filled
						if (j == row_count - 1) {
							j = 0;
							i++;
						}
					}
				}
			}

			return cofMat;
		}

		//std::enable_if_t<(row_count != 1 && column_count != 1), float>
		T determinant() const
		{
			static_assert(row_count == column_count);

			constexpr size_t n = row_count;

			// Base case for a 1x1 matrix
			if constexpr (row_count == 1 && column_count == 1)
				return GetElement(0, 0);
			else // it is important for this so the else clause doesn't compile for the row and column == 1 case
			{
				float D = 0; // Initialize determinant
				int sign = 1; // Alternating sign

				using MinorMatrix_t = MatrixGeneric<T, row_count - 1, column_count - 1>;
				// Loop through each element in the first row
				for (size_t f = 0; f < n; f++) {
					// Get the cofactor matrix excluding row 0 and column f
					MinorMatrix_t temp = GetCofactor(0, f);

					// Recursive expansion using the formula: D += sign * element * determinant(cofactor)
					D += sign * GetElement(0, f) * temp.determinant();

					// Alternate the sign for the next term
					sign = -sign;
				}

				return D; // Return the calculated determinant
			}
		}

		// std::enable_if_t<(row_count == 1 && column_count == 1), float>
		// determinant() const
		// {
		// 	return GetElement(0, 0);
		// }

		// template<typename TDet>
		// float determinant<TDet, 1, 1>(const MatrixGeneric<TDet, 1, 1>& mat) const
		// {
		// 	return mat.GetElement(0, 0);
		// }

		// template<typename TDet, size_t rows_det = 1, size_t cols_det = 1>
		// float determinant(const MatrixGeneric<TDet, rows_det, cols_det>& mat) const
		// {
		// 	return mat.GetElement(0, 0);
		// }

		MatrixGeneric getAdjoint() const {
			assert(row_count == column_count);
			size_t N = row_count;
			if (N == 1)
			{
				MatrixGeneric adj;
				adj[0][0] = 1; // Adjoint of a 1x1 matrix is simply 1
				return adj;
			}

			MatrixGeneric adj; // Create a matrix to store adjoint
			int sign = 1; // Sign alternates for each element

			for (size_t i = 0; i < N; i++)
			{
				for (size_t j = 0; j < N; j++)
				{
					// Calculate cofactor matrix for element (i, j)
					MatrixGeneric<T, row_count - 1, column_count - 1> temp = GetCofactor(i, j);

					// Determine the sign of the current element
					sign = ((i + j) % 2 == 0) ? 1 : -1;

					// Transpose while assigning adjoint (adj[j][i] instead of adj[i][j])
					adj[j][i] = sign * temp.determinant();
				}
			}
			return adj; // Return the computed adjoint matrix
		}


		MatrixGeneric GetInverse() const
		{
			float det = determinant();
			if (det == 0.f)
				throw std::runtime_error("Matrix is singular, inverse doesn't exist");

			MatrixGeneric adj = getAdjoint();
			MatrixGeneric inverse;

			for (size_t i = 0; i < row_count; i++)
				for (size_t j = 0; j < column_count; j++)
					inverse[i][j] = adj.rawMat[i][j] / det;

			return inverse;
		}

		T GetElement(size_t row, size_t column) const { return rawMat[row][column]; }

		union
		{
			T raw[row_count * column_count];
			T rawMat[row_count][column_count]; // [row][column]
		};

		friend std::ostream& operator<<(std::ostream& os, const MatrixGeneric& mat)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
					os << mat.rawMat[i][j] << ' ';

				os << "\n";
			}

			return os;
		}
	};

	template <typename T>
	class Matrix3x3 : public MatrixGeneric<T, 3, 3>
	{
		using BaseClassType = MatrixGeneric<T, 3, 3>;

	public:
		Matrix3x3() : BaseClassType() {}
		Matrix3x3(const BaseClassType& mat) : BaseClassType(mat) {}

		void SetColumn(int colIdx, Vec3<T> vec)
		{
			BaseClassType::SetColumn(colIdx, std::array<T, 3> {vec.x, vec.y, vec.z});
		}

		Vec3<T> GetColumn(int colIdx)
		{
			return Vec3<T>(BaseClassType::GetColumn(colIdx));
		}

		Vec3<T> operator*(const Vec3<T>& vec)
		{
			return BaseClassType::operator*(std::array<T, 3> {vec.x, vec.y, vec.z});
		}

		void SetRow(int rowIdx, Vec3<T> row)
		{
			BaseClassType::SetRow(rowIdx, std::array<T, 3> {row.x, row.y, row.z});
		}
	};

	using Mat3f = Matrix3x3<float>;
	using Mat3d = Matrix3x3<double>;

	template
		<typename T>
		class Matrix4x4 : public MatrixGeneric<T, 4, 4>
	{
	public:
		using BaseClassType = MatrixGeneric<T, 4, 4>;

		Matrix4x4() : BaseClassType() {}

		Matrix4x4(const BaseClassType& matGeneric) : BaseClassType(matGeneric) {}

		Matrix4x4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
		{
			BaseClassType::BaseClassType::rawMat[0][0] = m00; BaseClassType::rawMat[0][1] = m01; BaseClassType::rawMat[0][2] = m02; BaseClassType::rawMat[0][3] = m03;
			BaseClassType::rawMat[1][0] = m10; BaseClassType::rawMat[1][1] = m11; BaseClassType::rawMat[1][2] = m12; BaseClassType::rawMat[1][3] = m13;
			BaseClassType::rawMat[2][0] = m20; BaseClassType::rawMat[2][1] = m21; BaseClassType::rawMat[2][2] = m22; BaseClassType::rawMat[2][3] = m23;
			BaseClassType::rawMat[3][0] = m30; BaseClassType::rawMat[3][1] = m31; BaseClassType::rawMat[3][2] = m32; BaseClassType::rawMat[3][3] = m33;
		}

		Matrix4x4(Vector4<T> row0, Vector4<T> row1, Vector4<T> row2, Vector4<T> row3)
		{
			memcpy(BaseClassType::rawMat[0], row0.getRaw(), BaseClassType::row_size);
			memcpy(BaseClassType::rawMat[1], row1.getRaw(), BaseClassType::row_size);
			memcpy(BaseClassType::rawMat[2], row2.getRaw(), BaseClassType::row_size);
			memcpy(BaseClassType::rawMat[3], row3.getRaw(), BaseClassType::row_size);
		}

		Vector4<T> operator*(const Vector4<T>& vec)
		{
			return Vector4<T>
			{
				Vector4<T>{ BaseClassType::rawMat[0] }.dot(vec),
					Vector4<T>{ BaseClassType::rawMat[1] }.dot(vec),
					Vector4<T>{ BaseClassType::rawMat[2] }.dot(vec),
					Vector4<T>{ BaseClassType::rawMat[3] }.dot(vec)
			};
		}


		using BaseClassType::operator*;
		using BaseClassType::operator*=;

		Matrix4x4 operator*(const Matrix4x4& mat) { return BaseClassType::operator*(mat); }

		Matrix4x4 GetInverse()
		{
			// taken from glm
#define INVERSE_IMPLEMENTATION_GLM 0
#define INVERSE_IMPLEMENTATION_BYHAND 1
#define INVERSE_IMPLEMENTATION INVERSE_IMPLEMENTATION_BYHAND

#if INVERSE_IMPLEMENTATION == INVERSE_IMPLEMENTATION_GLM

			T Coef00 = BaseClassType::rawMat[2][2] * BaseClassType::rawMat[3][3] - BaseClassType::rawMat[3][2] * BaseClassType::rawMat[2][3];
			T Coef02 = BaseClassType::rawMat[1][2] * BaseClassType::rawMat[3][3] - BaseClassType::rawMat[3][2] * BaseClassType::rawMat[1][3];
			T Coef03 = BaseClassType::rawMat[1][2] * BaseClassType::rawMat[2][3] - BaseClassType::rawMat[2][2] * BaseClassType::rawMat[1][3];

			T Coef04 = BaseClassType::rawMat[2][1] * BaseClassType::rawMat[3][3] - BaseClassType::rawMat[3][1] * BaseClassType::rawMat[2][3];
			T Coef06 = BaseClassType::rawMat[1][1] * BaseClassType::rawMat[3][3] - BaseClassType::rawMat[3][1] * BaseClassType::rawMat[1][3];
			T Coef07 = BaseClassType::rawMat[1][1] * BaseClassType::rawMat[2][3] - BaseClassType::rawMat[2][1] * BaseClassType::rawMat[1][3];

			T Coef08 = BaseClassType::rawMat[2][1] * BaseClassType::rawMat[3][2] - BaseClassType::rawMat[3][1] * BaseClassType::rawMat[2][2];
			T Coef10 = BaseClassType::rawMat[1][1] * BaseClassType::rawMat[3][2] - BaseClassType::rawMat[3][1] * BaseClassType::rawMat[1][2];
			T Coef11 = BaseClassType::rawMat[1][1] * BaseClassType::rawMat[2][2] - BaseClassType::rawMat[2][1] * BaseClassType::rawMat[1][2];

			T Coef12 = BaseClassType::rawMat[2][0] * BaseClassType::rawMat[3][3] - BaseClassType::rawMat[3][0] * BaseClassType::rawMat[2][3];
			T Coef14 = BaseClassType::rawMat[1][0] * BaseClassType::rawMat[3][3] - BaseClassType::rawMat[3][0] * BaseClassType::rawMat[1][3];
			T Coef15 = BaseClassType::rawMat[1][0] * BaseClassType::rawMat[2][3] - BaseClassType::rawMat[2][0] * BaseClassType::rawMat[1][3];

			T Coef16 = BaseClassType::rawMat[2][0] * BaseClassType::rawMat[3][2] - BaseClassType::rawMat[3][0] * BaseClassType::rawMat[2][2];
			T Coef18 = BaseClassType::rawMat[1][0] * BaseClassType::rawMat[3][2] - BaseClassType::rawMat[3][0] * BaseClassType::rawMat[1][2];
			T Coef19 = BaseClassType::rawMat[1][0] * BaseClassType::rawMat[2][2] - BaseClassType::rawMat[2][0] * BaseClassType::rawMat[1][2];

			T Coef20 = BaseClassType::rawMat[2][0] * BaseClassType::rawMat[3][1] - BaseClassType::rawMat[3][0] * BaseClassType::rawMat[2][1];
			T Coef22 = BaseClassType::rawMat[1][0] * BaseClassType::rawMat[3][1] - BaseClassType::rawMat[3][0] * BaseClassType::rawMat[1][1];
			T Coef23 = BaseClassType::rawMat[1][0] * BaseClassType::rawMat[2][1] - BaseClassType::rawMat[2][0] * BaseClassType::rawMat[1][1];

			Vector4<T> Fac0(Coef00, Coef00, Coef02, Coef03);
			Vector4<T> Fac1(Coef04, Coef04, Coef06, Coef07);
			Vector4<T> Fac2(Coef08, Coef08, Coef10, Coef11);
			Vector4<T> Fac3(Coef12, Coef12, Coef14, Coef15);
			Vector4<T> Fac4(Coef16, Coef16, Coef18, Coef19);
			Vector4<T> Fac5(Coef20, Coef20, Coef22, Coef23);

			Vector4<T> Vec0(BaseClassType::rawMat[1][0], BaseClassType::rawMat[0][0], BaseClassType::rawMat[0][0], BaseClassType::rawMat[0][0]);
			Vector4<T> Vec1(BaseClassType::rawMat[1][1], BaseClassType::rawMat[0][1], BaseClassType::rawMat[0][1], BaseClassType::rawMat[0][1]);
			Vector4<T> Vec2(BaseClassType::rawMat[1][2], BaseClassType::rawMat[0][2], BaseClassType::rawMat[0][2], BaseClassType::rawMat[0][2]);
			Vector4<T> Vec3(BaseClassType::rawMat[1][3], BaseClassType::rawMat[0][3], BaseClassType::rawMat[0][3], BaseClassType::rawMat[0][3]);

			Vector4<T> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			Vector4<T> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			Vector4<T> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			Vector4<T> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			Vector4<T> SignA(+1, -1, +1, -1);
			Vector4<T> SignB(-1, +1, -1, +1);

			Matrix4x4<T> Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);
			Inverse = Inverse.GetTranspose();

			Vector4<T> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

			Vector4<T> Dot0(Vector4<T>(BaseClassType::rawMat[0]) * Row0);
			T Dot1 = (Dot0.x() + Dot0.y()) + (Dot0.z() + Dot0.w());

			T OneOverDeterminant = static_cast<T>(1) / Dot1;

			return Inverse * OneOverDeterminant;
#elif INVERSE_IMPLEMENTATION == INVERSE_IMPLEMENTATION_BYHAND
			return BaseClassType::GetInverse();

#endif

		}

		Matrix4x4 GetTranspose()
		{
			Matrix4x4 transposed;
			transposed.SetColumn(0, BaseClassType::rawMat[0]);
			transposed.SetColumn(1, BaseClassType::rawMat[1]);
			transposed.SetColumn(2, BaseClassType::rawMat[2]);
			transposed.SetColumn(3, BaseClassType::rawMat[3]);

			return transposed;
		}

		void SetColumn(int column_idx, Vector4<T> column)
		{
			BaseClassType::SetColumn(column_idx, std::to_array<T, BaseClassType::row_count>(column.getRaw()));
		}

		void SetRow(int rowIdx, const Vec3<T>& row)
		{
			memcpy(BaseClassType::rawMat[rowIdx], row.raw, 3 * sizeof(T));
		}

		void SetRow(int rowIdx, const Vector4<T>& row)
		{
			BaseClassType::SetRow(rowIdx, row.getRaw());
		}

	};



	using Vec4f = Vector4<float>;
	using Mat4 = Matrix4x4<float>;

}

#endif //__GEOMETRY_H__

