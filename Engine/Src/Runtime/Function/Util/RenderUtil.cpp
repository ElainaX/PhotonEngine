#include "RenderUtil.h"
#include "Macro.h"

#include <map>
#include <cmath>


namespace photon 
{

	Microsoft::WRL::ComPtr<ID3DBlob> RenderUtil::CreateD3DBlob(const void* data, UINT64 sizeInBytes, INT64 maxCopySize)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		DX_LogIfFailed(D3DCreateBlob(sizeInBytes, &blob));
		if (maxCopySize == -1)
			CopyMemory(blob->GetBufferPointer(), data, sizeInBytes);
		else
			CopyMemory(blob->GetBufferPointer(), data, sizeInBytes > sizeInBytes ? sizeInBytes : maxCopySize);
		return blob;
	}

	UINT RenderUtil::GetConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 256) & ~255;
	}

	UINT RenderUtil::FloatRoundToUINT(float a)
	{
		return (UINT)std::round(a);
	}

	std::pair<std::vector<float>, std::vector<float>> RenderUtil::LogLinearCascadedSplit(float cam_near, float cam_far, float shadow_distance, int cascadedCount, float lambda)
	{
		std::vector<float> spliter_distances;
		std::vector<float> spliter_ratios;
		assert(shadow_distance > cam_near);
		float farDivNear = shadow_distance / cam_near;
		float invCascadedCount = 1.0f / cascadedCount;
		for (int i = 1; i < cascadedCount; ++i)
		{
			float d = lambda * cam_near * std::powf(farDivNear, i * invCascadedCount)
				+ (1 - lambda) * (cam_near + (shadow_distance - cam_near) * (i * invCascadedCount));
			spliter_distances.push_back(d);
			spliter_ratios.push_back((d - cam_near) / (cam_far - cam_near));
		}
		spliter_distances.push_back(shadow_distance);
		spliter_ratios.push_back((shadow_distance - cam_near) / (cam_far - cam_near));

		return std::make_pair(spliter_distances, spliter_ratios);
	}

	std::vector<Vector3> RenderUtil::ProjViewToCorners(DirectX::XMMATRIX proj, DirectX::XMMATRIX view, bool reversedZ)
	{
		using namespace DirectX;

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);

		// D3D NDC: x,y in [-1,1], z in [0,1]
		float zNear = reversedZ ? 1.0f : 0.0f;
		float zFar = reversedZ ? 0.0f : 1.0f;

		XMVECTOR ndc[8] = {
			XMVectorSet(-1, -1, zNear, 1), // near
			XMVectorSet(-1, +1, zNear, 1),
			XMVectorSet(+1, +1, zNear, 1),
			XMVectorSet(+1, -1, zNear, 1),

			XMVectorSet(-1, -1, zFar, 1),  // far
			XMVectorSet(-1, +1, zFar, 1),
			XMVectorSet(+1, +1, zFar, 1),
			XMVectorSet(+1, -1, zFar, 1),
		};

		std::vector<Vector3> out(8);
		for (int i = 0; i < 8; ++i) {
			XMVECTOR p = XMVector4Transform(ndc[i], invViewProj);
			p = XMVectorScale(p, 1.0f / XMVectorGetW(p)); // divide by w
			XMStoreFloat3((XMFLOAT3*)&out[i], p);
		}
		return out;
	}
}
