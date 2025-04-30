#pragma once

#include <array>
#include <cmath>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "Vector4.h"

namespace photon 
{
	constexpr float M_PI = 3.141592657f;
	// near plane ~ far plane
	const Vector4 NdcCorners[8] = {
		Vector4{-1.0f, -1.0f, 0.0f, 1.0f}, Vector4{-1.0f, 1.0f, 0.0f, 1.0f},
		Vector4{1.0f, 1.0f, 0.0f, 1.0f}, Vector4{1.0f, -1.0f, 0.0f, 1.0f},
		Vector4{-1.0f, -1.0f, 1.0f, 1.0f}, Vector4{-1.0f, 1.0f, 1.0f, 1.0f},
		Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector4{1.0f, -1.0f, 1.0f, 1.0f}
	};

	inline DirectX::XMMATRIX DXFloat3x3ToMatrix(const DirectX::XMFLOAT3X3& float3x3)
	{
		using namespace DirectX;
	
		float rawFloat[16] = {
			float3x3._11, float3x3._12, float3x3._13, 0.0f,
			float3x3._21, float3x3._22, float3x3._23, 0.0f,
			float3x3._31, float3x3._32, float3x3._33, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		return XMMATRIX(rawFloat);
	}

	inline Vector3 toVector3(const Vector4& vec)
	{
		return Vector3{ vec.x, vec.y, vec.z };
	}


	inline std::array<Vector4, 8> GetFrustumCornerWorld(const DirectX::XMMATRIX& invProj, const DirectX::XMMATRIX& invView)
	{
		using namespace DirectX;
		std::array<Vector4, 8> worldCorners;
		for(int i = 0; i < 8; ++i)
		{
			XMVECTOR corner = XMLoadFloat4((XMFLOAT4*)&NdcCorners[i]);
			auto viewCorner = XMVector4Transform(corner, invProj);
			auto worldCorner = XMVector4Transform(viewCorner, invView);
			XMStoreFloat4((XMFLOAT4*)&worldCorners[i], worldCorner);
			float z = static_cast<float>(1.0 / (double)worldCorners[i].w);
			worldCorners[i] *= z;
		}
		return worldCorners;
	}

}