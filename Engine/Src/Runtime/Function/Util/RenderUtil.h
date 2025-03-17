#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>

namespace photon 
{
	class RenderUtil
	{
	public:
		static Microsoft::WRL::ComPtr<ID3DBlob> CreateD3DBlob(const void* data, UINT64 sizeInBytes);
	};


}