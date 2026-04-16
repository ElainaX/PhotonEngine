#pragma once
#include "Vector3i.h"

namespace photon
{

	class Vector4i
	{
	public:
		int x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 };

	public:
		Vector4i() = default;
		Vector4i(int x_, int y_, int z_, int w_) : x{ x_ }, y{ y_ }, z{ z_ }, w{ w_ } {}
		Vector4i(const Vector3i& v3, int w_) : x{ v3.x }, y{ v3.y }, z{ v3.z }, w{ w_ } {}

		explicit Vector4i(int coords[4]) : x{ coords[0] }, y{ coords[1] }, z{ coords[2] }, w{ coords[3] } {}

		int operator[](size_t i) const
		{
			assert(i < 4);
			return *(&x + i);
		}

		int& operator[](size_t i)
		{
			assert(i < 4);
			return *(&x + i);
		}

		/// Pointer accessor for direct copying
		int* ptr() { return &x; }
		/// Pointer accessor for direct copying
		const int* ptr() const { return &x; }

		Vector4i& operator=(int scalar)
		{
			x = scalar;
			y = scalar;
			z = scalar;
			w = scalar;
			return *this;
		}

		bool operator==(const Vector4i& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w); }

		bool operator!=(const Vector4i& rhs) const { return !(rhs == *this); }

		Vector4i operator+(const Vector4i& rhs) const { return Vector4i(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
		Vector4i operator-(const Vector4i& rhs) const { return Vector4i(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
		Vector4i operator*(int scalar) const { return Vector4i(x * scalar, y * scalar, z * scalar, w * scalar); }
		Vector4i operator*(const Vector4i& rhs) const { return Vector4i(rhs.x * x, rhs.y * y, rhs.z * z, rhs.w * w); }
		Vector4i operator/(int scalar) const
		{
			assert(scalar != 0.0);
			return Vector4i(x / scalar, y / scalar, z / scalar, w / scalar);
		}
		Vector4i operator/(const Vector4i& rhs) const
		{
			assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
			return Vector4i(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}

		const Vector4i& operator+() const { return *this; }

		Vector4i operator-() const { return Vector4i(-x, -y, -z, -w); }

		friend Vector4i operator*(int scalar, const Vector4i& rhs)
		{
			return Vector4i(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w);
		}

		friend Vector4i operator/(int scalar, const Vector4i& rhs)
		{
			assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
			return Vector4i(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
		}

		friend Vector4i operator+(const Vector4i& lhs, int rhs)
		{
			return Vector4i(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
		}

		friend Vector4i operator+(int lhs, const Vector4i& rhs)
		{
			return Vector4i(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
		}

		friend Vector4i operator-(const Vector4i& lhs, int rhs)
		{
			return Vector4i(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
		}

		friend Vector4i operator-(int lhs, const Vector4i& rhs)
		{
			return Vector4i(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
		}

		// arithmetic updates
		Vector4i& operator+=(const Vector4i& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}

		Vector4i& operator-=(const Vector4i& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}

		Vector4i& operator*=(int scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}

		Vector4i& operator+=(int scalar)
		{
			x += scalar;
			y += scalar;
			z += scalar;
			w += scalar;
			return *this;
		}

		Vector4i& operator-=(int scalar)
		{
			x -= scalar;
			y -= scalar;
			z -= scalar;
			w -= scalar;
			return *this;
		}

		Vector4i& operator*=(const Vector4i& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}

		Vector4i& operator/=(int scalar)
		{
			assert(scalar != 0.0);

			x /= scalar;
			y /= scalar;
			z /= scalar;
			w /= scalar;
			return *this;
		}

		Vector4i& operator/=(const Vector4i& rhs)
		{
			assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}

		/** Calculates the dot (scalar) product of this vector with another.
		@param
		vec Vector with which to calculate the dot product (together
		with this one).
		@returns
		A int representing the dot product value.
		*/
		int dotProduct(const Vector4i& vec) const { return x * vec.x + y * vec.y + z * vec.z + w * vec.w; }


		// special
		static const Vector4i ZERO;
		static const Vector4i UNIT_SCALE;
	};
}
