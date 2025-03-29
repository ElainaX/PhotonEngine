#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <tuple>
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"


namespace photon 
{
	// 四元数规定只能构造单位四元数
	class Quaternion 
	{
	public:
		Quaternion() = default;
		Quaternion(float _x, float _y, float _z, float _r) : x(_x), y(_y), z(_z), r(_r) {}
		Quaternion(const DirectX::XMFLOAT4& quat) : x(quat.x), y(quat.y), z(quat.z), r(quat.w) {}
		// 通过旋转来构造Quaternion
		// 这里Quaternion只接受
		Quaternion(float angleInRadians, const Vector3& axis) { this->ConstructByAngleAxis(angleInRadians, axis); }
		Quaternion(const DirectX::XMFLOAT3X3& dxMat) { this->ConstructByDXMatrix(dxMat); }
		Quaternion(const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ) { this->ConstructByBasisAxes(axisX, axisY, axisZ); }
		Quaternion(float pitch, float yaw, float roll = 0.0f) { this->ConstructByRawYawPitch(pitch, yaw, roll); }

		void ConstructByAngleAxis(float angleInRadians, const Vector3& axis);
		void ConstructByDXMatrix(const DirectX::XMFLOAT3X3& dxMat);
		void ConstructByBasisAxes(const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ);
		void ConstructByRawYawPitch(float pitch, float yaw, float roll = 0.0f);

		DirectX::XMMATRIX ToRotationMatrix() const;
		std::tuple<float, Vector3> ToAngleAxis() const;
		std::tuple<Vector3, Vector3, Vector3> ToBasisAxes() const;

		Quaternion operator+(const Quaternion& rhs) const
		{
			return Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, r + rhs.r);
		}

		Quaternion operator-(const Quaternion& rhs) const
		{
			return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, r - rhs.r);
		}

		Quaternion mul(const Quaternion& rhs) const { return (*this) * rhs; }
		Quaternion operator*(const Quaternion& rhs) const;

		Quaternion operator*(float scalar) const { return Quaternion(x * scalar, y * scalar, z * scalar, r * scalar); }

		Quaternion operator/(float scalar) const
		{
			assert(scalar != 0.0f);
			return Quaternion(x / scalar, y / scalar, z / scalar, r / scalar);
		}

		friend Quaternion operator*(float scalar, const Quaternion& rhs)
		{
			return Quaternion(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.r);
		}
		
		Quaternion operator-() const { return Quaternion(-x, -y, -z, -r); }

		bool operator==(const Quaternion& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.r == r);
		}

		bool operator!=(const Quaternion& rhs) const
		{
			return (rhs.x != x) || (rhs.y != y) || (rhs.z != z) || (rhs.r != r);
		}

		float GetX() const { return x; }
		float GetY() const { return y; }
		float GetZ() const { return z; }
		float GetR() const { return r; }

		float Dot(const Quaternion& rkQ) const;
		float Length() const;
		Quaternion Inverse() const;

		DirectX::XMFLOAT4 ToDXFloat4() const
		{
			return DirectX::XMFLOAT4(x, y, z, r);
		}

		float* ptr() { return &x; }
		const float* const_ptr() const { return &x; }

		static Quaternion Lerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortes_path = true);
		Quaternion conjugate() const { return Quaternion(-x, -y, -z, r); }

		float x{ 0.0f }, y{ 0.0f }, z{0.0f}, r{ 1.0f };

		// special values
		static const Quaternion ZERO;
		static const Quaternion IDENTITY;

	private:

	};


}
