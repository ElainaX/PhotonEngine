#include "DXTexture.h"


namespace photon
{
	bool DXTexture::Initialize(const DXTextureDesc& desc, DXResourceDimension dim, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes, const std::string&
	                           _name)
	{
		hasClearValue = desc.hasClearValue;
		clearValue = desc.clearValue;
		return DXResource::Initialize(dim, _name, desc.heapProp,
		                              GetInitialResourceState(desc), ToDxDesc(desc), &clearValue, _gpuRes);
	}

	D3D12_RESOURCE_DESC DXTexture::ToDxDesc(const DXTextureDesc& desc)
	{
		D3D12_RESOURCE_DESC dxDesc;
		dxDesc.Dimension = desc.resDim;
		dxDesc.Alignment = 0;
		dxDesc.Width = desc.width;
		dxDesc.Height = desc.height;
		dxDesc.DepthOrArraySize = desc.depthOrArraySize;
		dxDesc.MipLevels = desc.mipLevels;
		dxDesc.Format = desc.format;
		dxDesc.SampleDesc.Count = desc.sampleCount;
		dxDesc.SampleDesc.Quality = desc.sampleQuality;
		dxDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dxDesc.Flags = desc.flag;
		return dxDesc;
	}

	D3D12_RESOURCE_FLAGS DXTexture::GetResourceFlags(const DXTextureDesc& desc)
	{
		return desc.flag;
	}

	D3D12_RESOURCE_STATES DXTexture::GetInitialResourceState(const DXTextureDesc& desc)
	{
		switch (desc.heapProp)
		{
		case HeapProp::Default:
			return D3D12_RESOURCE_STATE_COMMON;
		case HeapProp::Upload:
			return D3D12_RESOURCE_STATE_GENERIC_READ;
		case HeapProp::Readback:
			return D3D12_RESOURCE_STATE_COPY_DEST;
		case HeapProp::Unknown:
			PHOTON_ASSERT(false, "HeapProp 不能是 Unknown！");
		}
		return {};
	}
}
