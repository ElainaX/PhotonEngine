#pragma once

#include "DXResource.h"

namespace photon
{

	class DXTexture : public DXResource
	{
	public:
		DXTexture() = default;
		bool Initialize(const DXTextureDesc& desc, DXResourceDimension dim, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes, const std::string&
		                _name);


		static D3D12_RESOURCE_DESC ToDxDesc(const DXTextureDesc& desc);
		static D3D12_RESOURCE_FLAGS GetResourceFlags(const DXTextureDesc& desc);
		static D3D12_RESOURCE_STATES GetInitialResourceState(const DXTextureDesc& desc);

		bool hasClearValue = false;
		D3D12_CLEAR_VALUE clearValue = {};
	};
}

