#pragma once
#include "DXTexture2D.h"


namespace photon
{


	class DXTexture2DArray : public DXTexture
	{
	public:
		DXTexture2DArray() = default;
		bool Initialize(const DXTexture2DArrayDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes, const std::string& _name);

		static DXTextureDesc ToDXTextureDesc(const DXTexture2DArrayDesc& desc);
		static D3D12_RESOURCE_DESC ToDxDesc(const DXTexture2DArrayDesc& desc);
		static D3D12_RESOURCE_FLAGS GetResourceFlags(const DXTexture2DArrayDesc& desc);
		static D3D12_RESOURCE_STATES GetInitialResourceState(const DXTexture2DArrayDesc& desc);
		static DXResourceDimension GetResourceDim();
	};
}