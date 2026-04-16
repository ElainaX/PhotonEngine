#include "DXTexture2D.h"

namespace photon 
{
	bool DXTexture2D::Initialize(const DXTexture2DDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes,
	                             const std::string& _name)
	{
		DXTextureDesc texdesc = ToDXTextureDesc(desc);
		return DXTexture::Initialize(texdesc, GetResourceDim(), _gpuRes, _name);
	}

	DXTextureDesc DXTexture2D::ToDXTextureDesc(const DXTexture2DDesc& desc)
	{
		DXTextureDesc texDesc = {};
		texDesc.width = desc.width;
		texDesc.height = desc.height;
		texDesc.depthOrArraySize = 1;
		texDesc.mipLevels = desc.maxMipLevels;
		texDesc.format = desc.format;
		texDesc.flag = desc.flag;
		texDesc.resDim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.heapProp = desc.heapProp;
		texDesc.hasClearValue = desc.hasClearValue;
		texDesc.clearValue.Format = desc.format;
		texDesc.clearValue.Color[0] = desc.clearValue.x;
		texDesc.clearValue.Color[1] = desc.clearValue.y;
		texDesc.clearValue.Color[2] = desc.clearValue.z;
		texDesc.clearValue.Color[3] = desc.clearValue.w;
		return texDesc;
	}

	D3D12_RESOURCE_DESC DXTexture2D::ToDxDesc(const DXTexture2DDesc& desc)
	{
		return DXTexture::ToDxDesc(ToDXTextureDesc(desc));
	}

	D3D12_RESOURCE_FLAGS DXTexture2D::GetResourceFlags(const DXTexture2DDesc& desc)
	{
		return desc.flag;
	}

	D3D12_RESOURCE_STATES DXTexture2D::GetInitialResourceState(const DXTexture2DDesc& desc)
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

	DXResourceDimension DXTexture2D::GetResourceDim()
	{
		return DXResourceDimension::Texture2D;
	}
}
