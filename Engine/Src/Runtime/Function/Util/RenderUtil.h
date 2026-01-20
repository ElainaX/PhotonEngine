#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <wrl.h>
#include <vector>
#include <utility>

#include "Core/Math/Vector4.h"

namespace photon 
{

	class RenderUtil
	{
	public:
		static Microsoft::WRL::ComPtr<ID3DBlob> CreateD3DBlob(const void* data, UINT64 sizeInBytes, INT64 maxCopySize = -1);

		// constant buffer必须256B对齐
		static UINT GetConstantBufferByteSize(UINT byteSize);

		static UINT FloatRoundToUINT(float a);

		// first = 相机空间的distance的spliters，元素的数值在cam_near ~ shadow_distance之间
		// second = 根据frustum比例的分割线，元素的数值在0~1之间
		static std::pair<std::vector<float>, std::vector<float>> LogLinearCascadedSplit(float cam_near, float cam_far, float shadow_distance, int cascadedCount, float lambda = 0.5f);

		static std::vector<Vector3> ProjViewToCorners(DirectX::XMMATRIX proj, DirectX::XMMATRIX view, bool reversedZ = false);
	};


}