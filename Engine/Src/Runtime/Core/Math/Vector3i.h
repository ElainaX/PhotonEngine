#pragma once

#include <cassert>
#include <cmath>
#include <algorithm>

#include <DirectXMath.h>

namespace photon
{
	class Vector3i
	{
	public:
		int x{ 0 };
		int y{ 0 };
		int z{ 0 };

	public:
		Vector3i() = default;
		Vector3i(int x_, int y_, int z_) : x{ x_ }, y{ y_ }, z{ z_ } {}

		explicit Vector3i(const int coords[3]) : x{ coords[0] }, y{ coords[1] }, z{ coords[2] } {}

		int operator[](size_t i) const
		{
			assert(i < 3);
			return *(&x + i);
		}

		int& operator[](size_t i)
		{
			assert(i < 3);
			return *(&x + i);
		}
		/// Pointer accessor for direct copying
		int* ptr() { return &x; }
		/// Pointer accessor for direct copying
		const int* ptr() const { return &x; }

		bool operator==(const Vector3i& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z); }

		bool operator!=(const Vector3i& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

		// arithmetic operations
		Vector3i operator+(const Vector3i& rhs) const { return Vector3i(x + rhs.x, y + rhs.y, z + rhs.z); }

		Vector3i operator-(const Vector3i& rhs) const { return Vector3i(x - rhs.x, y - rhs.y, z - rhs.z); }

		Vector3i operator*(int scalar) const { return Vector3i(x * scalar, y * scalar, z * scalar); }

		Vector3i operator*(const Vector3i& rhs) const { return Vector3i(x * rhs.x, y * rhs.y, z * rhs.z); }

		Vector3i operator/(int scalar) const
		{
			assert(scalar != 0.0);
			return Vector3i(x / scalar, y / scalar, z / scalar);
		}

		Vector3i operator/(const Vector3i& rhs) const
		{
			assert((rhs.x != 0 && rhs.y != 0 && rhs.z != 0));
			return Vector3i(x / rhs.x, y / rhs.y, z / rhs.z);
		}

		const Vector3i& operator+() const { return *this; }

		Vector3i operator-() const { return Vector3i(-x, -y, -z); }

		// overloaded operators to help Vector3i
		friend Vector3i operator*(int scalar, const Vector3i& rhs)
		{
			return Vector3i(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
		}

		friend Vector3i operator/(int scalar, const Vector3i& rhs)
		{
			assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
			return Vector3i(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
		}

		friend Vector3i operator+(const Vector3i& lhs, int rhs)
		{
			return Vector3i(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
		}

		friend Vector3i operator+(int lhs, const Vector3i& rhs)
		{
			return Vector3i(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
		}

		friend Vector3i operator-(const Vector3i& lhs, int rhs)
		{
			return Vector3i(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
		}

		friend Vector3i operator-(int lhs, const Vector3i& rhs)
		{
			return Vector3i(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
		}

		// arithmetic updates
		Vector3i& operator+=(const Vector3i& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		Vector3i& operator+=(int scalar)
		{
			x += scalar;
			y += scalar;
			z += scalar;
			return *this;
		}

		Vector3i& operator-=(const Vector3i& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		Vector3i& operator-=(int scalar)
		{
			x -= scalar;
			y -= scalar;
			z -= scalar;
			return *this;
		}

		Vector3i& operator*=(int scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		Vector3i& operator*=(const Vector3i& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		Vector3i& operator/=(int scalar)
		{
			assert(scalar != 0.0);
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}

		Vector3i& operator/=(const Vector3i& rhs)
		{
			assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		/** Returns the square of the length(magnitude) of the vector.
		@remarks
		This  method is for efficiency - calculating the actual
		length of a vector requires a square root, which is expensive
		in terms of the operations required. This method returns the
		square of the length of the vector, i.e. the same as the
		length but before the square root is taken. Use this if you
		want to find the longest / shortest vector without incurring
		the square root.
		*/
		int squaredLength() const { return x * x + y * y + z * z; }

		
		/** Returns the square of the distance to another vector.
		@remarks
		This method is for efficiency - calculating the actual
		distance to another vector requires a square root, which is
		expensive in terms of the operations required. This method
		returns the square of the distance to another vector, i.e.
		the same as the distance but before the square root is taken.
		Use this if you want to find the longest / shortest distance
		without incurring the square root.
		*/

		int squaredDistance(const Vector3i& rhs) const { return (*this - rhs).squaredLength(); }

		/** Calculates the dot (scalar) product of this vector with another.
		@remarks
		The dot product can be used to calculate the angle between 2
		vectors. If both are unit vectors, the dot product is the
		cosine of the angle; otherwise the dot product must be
		divided by the product of the lengths of both vectors to get
		the cosine of the angle. This result can further be used to
		calculate the distance of a point from a plane.
		@param
		vec Vector with which to calculate the dot product (together
		with this one).
		@returns
		A int representing the dot product value.
		*/

		int dotProduct(const Vector3i& vec) const { return x * vec.x + y * vec.y + z * vec.z; }

		/** Normalizes the vector.
		@remarks
		This method normalizes the vector such that it's
		length / magnitude is 1. The result is called a unit vector.
		@note
		This function will not crash for zero-sized vectors, but there
		will be no changes made to their components.
		@returns The previous length of the vector.
		*/

		//void normalise()
		//{
		//	int length = std::hypot(x, y, z);
		//	if (length == 0.f)
		//		return;

		//	int inv_lengh = 1.0f / length;
		//	x *= inv_lengh;
		//	y *= inv_lengh;
		//	z *= inv_lengh;
		//}

		/** Calculates the cross-product of 2 vectors, i.e. the vector that
		lies perpendicular to them both.
		@remarks
		The cross-product is normally used to calculate the normal
		vector of a plane, by calculating the cross-product of 2
		non-equivalent vectors which lie on the plane (e.g. 2 edges
		of a triangle).
		@param
		vec Vector which, together with this one, will be used to
		calculate the cross-product.
		@returns
		A vector which is the result of the cross-product. This
		vector will <b>NOT</b> be normalised, to maximize efficiency
		- call Vector3i::normalise on the result if you wish this to
		be done. As for which side the resultant vector will be on, the
		returned vector will be on the side from which the arc from 'this'
		to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
		= UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
		This is because CHAOS uses a right-handed coordinate system.
		@par
		For a clearer explanation, look a the left and the bottom edges
		of your monitor's screen. Assume that the first vector is the
		left edge and the second vector is the bottom edge, both of
		them starting from the lower-left corner of the screen. The
		resulting vector is going to be perpendicular to both of them
		and will go <i>inside</i> the screen, towards the cathode tube
		(assuming you're using a CRT monitor, of course).
		*/

		Vector3i crossProduct(const Vector3i& rhs) const
		{
			return Vector3i(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
		}

		/** Sets this vector's components to the minimum of its own and the
		ones of the passed in vector.
		@remarks
		'Minimum' in this case means the combination of the lowest
		value of x, y and z from both vectors. Lowest is taken just
		numerically, not magnitude, so -1 < 0.
		*/
		void makeFloor(const Vector3i& cmp)
		{
			if (cmp.x < x)
				x = cmp.x;
			if (cmp.y < y)
				y = cmp.y;
			if (cmp.z < z)
				z = cmp.z;
		}

		/** Sets this vector's components to the maximum of its own and the
		ones of the passed in vector.
		@remarks
		'Maximum' in this case means the combination of the highest
		value of x, y and z from both vectors. Highest is taken just
		numerically, not magnitude, so 1 > -3.
		*/
		void makeCeil(const Vector3i& cmp)
		{
			if (cmp.x > x)
				x = cmp.x;
			if (cmp.y > y)
				y = cmp.y;
			if (cmp.z > z)
				z = cmp.z;
		}

		/** Returns true if this vector is zero length. */
		bool isZeroLength(void) const
		{
			int sqlen = (x * x) + (y * y) + (z * z);
			return (sqlen < (1e-06 * 1e-06));
		}

		bool isZero() const { return x == 0.f && y == 0.f && z == 0.f; }

		/** As normalise, except that this vector is unaffected and the
		normalised vector is returned as a copy. */

		//Vector3i normalisedCopy(void) const
		//{
		//	Vector3i ret = *this;
		//	ret.normalise();
		//	return ret;
		//}

		/** Calculates a reflection vector to the plane with the given normal .
		@remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
		*/
		Vector3i reflect(const Vector3i& normal) const
		{
			return Vector3i(*this - (2 * this->dotProduct(normal) * normal));
		}

		/** Calculates projection to a plane with the given normal
		@param normal The normal of given plane
		*/
		Vector3i project(const Vector3i& normal) const { return Vector3i(*this - (this->dotProduct(normal) * normal)); }

		Vector3i absoluteCopy() const { return Vector3i(fabsf(x), fabsf(y), fabsf(z)); }

		static Vector3i lerp(const Vector3i& lhs, const Vector3i& rhs, int alpha) { return lhs + alpha * (rhs - lhs); }

		static Vector3i clamp(const Vector3i& v, const Vector3i& min, const Vector3i& max)
		{
			return Vector3i(
				std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z));
		}

		// special points
		static const Vector3i ZERO;
		static const Vector3i UNIT_X;
		static const Vector3i UNIT_Y;
		static const Vector3i UNIT_Z;
		static const Vector3i NEGATIVE_UNIT_X;
		static const Vector3i NEGATIVE_UNIT_Y;
		static const Vector3i NEGATIVE_UNIT_Z;
		static const Vector3i UNIT_SCALE;
	};
} // namespace Piccolo
