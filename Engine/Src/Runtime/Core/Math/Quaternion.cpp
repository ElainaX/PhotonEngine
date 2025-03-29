#include "Quaternion.h"
#include "MathFunction.h"


namespace photon 
{
	using namespace DirectX;

	const Quaternion Quaternion::ZERO(0, 0, 0, 0);
	const Quaternion Quaternion::IDENTITY(0, 0, 0, 1);

	void Quaternion::ConstructByAngleAxis(float angleInRadians, const Vector3& axis)
	{
		XMVECTOR vecAxis = XMLoadFloat3((const XMFLOAT3*)axis.ptr());
		XMVECTOR result = XMQuaternionRotationAxis(vecAxis, angleInRadians);
		XMStoreFloat4((XMFLOAT4*)ptr(), result);
	}

	void Quaternion::ConstructByDXMatrix(const DirectX::XMFLOAT3X3& dxMat)
	{
		XMMATRIX mat = DXFloat3x3ToMatrix(dxMat);
		XMVECTOR vec = XMQuaternionRotationMatrix(mat);
		XMStoreFloat4((XMFLOAT4*)ptr(), vec);
	}

	void Quaternion::ConstructByBasisAxes(const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ)
	{
		XMMATRIX mat = XMMatrixIdentity();
		mat.r[0] = XMLoadFloat3((const XMFLOAT3*)&axisX);
		mat.r[1] = XMLoadFloat3((const XMFLOAT3*)&axisY);
		mat.r[2] = XMLoadFloat3((const XMFLOAT3*)&axisZ);
		XMVECTOR vec = XMQuaternionRotationMatrix(mat);
		XMStoreFloat4((XMFLOAT4*)ptr(), vec);
	}

	void Quaternion::ConstructByRawYawPitch(float pitch, float yaw, float roll /*= 0.0f*/)
	{
		XMVECTOR vec = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		XMStoreFloat4((XMFLOAT4*)ptr(), vec);
	}

	DirectX::XMMATRIX Quaternion::ToRotationMatrix() const
	{
		XMVECTOR vec = XMLoadFloat4((const XMFLOAT4*)const_ptr());
		return XMMatrixRotationQuaternion(vec);
	}

	std::tuple<float, photon::Vector3> Quaternion::ToAngleAxis() const
	{
		Vector3 axis{ x, y, z };
		axis.normalise();
		float angle = std::acosf(r);
		return std::make_tuple(r, axis);
	}

	std::tuple<photon::Vector3, photon::Vector3, photon::Vector3> Quaternion::ToBasisAxes() const
	{
		XMMATRIX mat = ToRotationMatrix();
		Vector3 axisX, axisY, axisZ;
		XMStoreFloat3((XMFLOAT3*)&axisX, mat.r[0]);
		XMStoreFloat3((XMFLOAT3*)&axisY, mat.r[1]);
		XMStoreFloat3((XMFLOAT3*)&axisZ, mat.r[2]);
		return std::make_tuple(axisX, axisY, axisZ);
	}

	photon::Quaternion Quaternion::operator*(const Quaternion& rhs) const
	{
		XMVECTOR vec = XMQuaternionMultiply(XMLoadFloat4((const XMFLOAT4*)const_ptr()), XMLoadFloat4((const XMFLOAT4*)rhs.const_ptr()));
		XMFLOAT4 dxFloat4;
		XMStoreFloat4(&dxFloat4, vec);
		return Quaternion(dxFloat4);
	}

	float Quaternion::Dot(const Quaternion& rkQ) const
	{
		XMVECTOR vec = XMQuaternionDot(XMLoadFloat4((const XMFLOAT4*)const_ptr()), XMLoadFloat4((const XMFLOAT4*)rkQ.const_ptr()));
		float dxFloat;
		XMStoreFloat(&dxFloat, vec);
		return dxFloat;
	}

	float Quaternion::Length() const
	{
		XMVECTOR vec = XMQuaternionLength(XMLoadFloat4((const XMFLOAT4*)const_ptr()));
		float length;
		XMStoreFloat(&length, vec);
		return length;
	}

	photon::Quaternion Quaternion::Inverse() const
	{
		XMVECTOR vec = XMQuaternionInverse(XMLoadFloat4((const XMFLOAT4*)const_ptr()));
		XMFLOAT4 dxFloat4;
		XMStoreFloat4(&dxFloat4, vec);
		return Quaternion(dxFloat4);
	}

	photon::Quaternion Quaternion::Lerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path /*= true*/)
	{
		XMFLOAT4 dxFloat4;

		if(shortest_path)
		{
			XMVECTOR vec = XMQuaternionSlerp(XMLoadFloat4((const XMFLOAT4*)kp.const_ptr()),
				XMLoadFloat4((const XMFLOAT4*)kq.const_ptr()), t);
			XMStoreFloat4(&dxFloat4, vec);
			return Quaternion(dxFloat4);
		}
		else 
		{
			float theta = (float)std::acosf(kp.Dot(kq));
			float alpha = XM_2PI - theta;
			float sinAlpha = std::sinf(alpha);
			float c1 = sin((1 - t) * alpha);
			float c2 = sin(t * alpha);
			return (c1 * kp + c2 * kq) / sinAlpha;
		}
	}

}