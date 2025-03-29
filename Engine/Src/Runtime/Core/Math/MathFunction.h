#pragma once

#include <cmath>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace photon 
{
	constexpr float M_PI = 3.141592657f;

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


}