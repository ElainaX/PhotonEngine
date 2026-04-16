#pragma once 

#include <wrl.h>
#include <d3d12.h>

#include "DXTexture.h"

namespace photon
{



	class DXTexture3D : public DXTexture
	{
	public:
		DXTexture3D() = default;
		bool Initialize(const DXTexture3DDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes, const std::string& _name);

		static DXTextureDesc ToDXTextureDesc(const DXTexture3DDesc& desc);
		static D3D12_RESOURCE_DESC ToDxDesc(const DXTexture3DDesc& desc);
		static D3D12_RESOURCE_FLAGS GetResourceFlags(const DXTexture3DDesc& desc);
		static D3D12_RESOURCE_STATES GetInitialResourceState(const DXTexture3DDesc& desc);
		static DXResourceDimension GetResourceDim();

	};
}