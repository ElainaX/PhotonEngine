#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <wrl.h>

namespace photon 
{
	class RenderUtil
	{
	public:
		static Microsoft::WRL::ComPtr<ID3DBlob> CreateD3DBlob(const void* data, UINT64 sizeInBytes, INT64 maxCopySize = -1);

		// constant buffer必须256B对齐
		static UINT GetConstantBufferByteSize(UINT byteSize);

		static UINT FloatRoundToUINT(float a);
	};


}