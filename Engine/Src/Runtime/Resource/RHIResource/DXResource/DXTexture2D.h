#pragma once 

#include <wrl.h>
#include <d3d12.h>

#include "DXTexture.h"

namespace photon
{



	class DXTexture2D : public DXTexture
	{
	public:
		DXTexture2D() = default;
		bool Initialize(const DXTexture2DDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes, const std::string& _name);

		static DXTextureDesc ToDXTextureDesc(const DXTexture2DDesc& desc);
		static D3D12_RESOURCE_DESC ToDxDesc(const DXTexture2DDesc& desc);
		static D3D12_RESOURCE_FLAGS GetResourceFlags(const DXTexture2DDesc& desc);
		static D3D12_RESOURCE_STATES GetInitialResourceState(const DXTexture2DDesc& desc);
		static DXResourceDimension GetResourceDim();

	};
}