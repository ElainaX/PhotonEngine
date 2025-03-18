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

		static D3D12_SRV_DIMENSION GetResourceSrvDimension(D3D12_RESOURCE_DESC desc);
	};


}