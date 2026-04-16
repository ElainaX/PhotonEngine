#include "DXTexture3D.h"

namespace photon
{
	bool DXTexture3D::Initialize(const DXTexture3DDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes,
	                             const std::string& _name)
	{
		DXTextureDesc texdesc = ToDXTextureDesc(desc);
		return DXTexture::Initialize(texdesc, GetResourceDim(), _gpuRes, _name);
	}

	DXTextureDesc DXTexture3D::ToDXTextureDesc(const DXTexture3DDesc& desc)
	{
		DXTextureDesc texDesc = {};
		texDesc.width = desc.width;
		texDesc.height = desc.height;
		texDesc.depthOrArraySize = desc.depth;
		texDesc.mipLevels = desc.maxMipLevels;
		texDesc.format = desc.format;
		texDesc.flag = desc.flag;
		texDesc.resDim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		texDesc.heapProp = desc.heapProp;
		texDesc.hasClearValue = false;
		texDesc.clearValue.Format = desc.format;
		return texDesc;
	}

	D3D12_RESOURCE_DESC DXTexture3D::ToDxDesc(const DXTexture3DDesc& desc)
	{
		return DXTexture::ToDxDesc(ToDXTextureDesc(desc));
	}

	D3D12_RESOURCE_FLAGS DXTexture3D::GetResourceFlags(const DXTexture3DDesc& desc)
	{
		return desc.flag;
	}

	D3D12_RESOURCE_STATES DXTexture3D::GetInitialResourceState(const DXTexture3DDesc& desc)
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

	DXResourceDimension DXTexture3D::GetResourceDim()
	{
		return DXResourceDimension::Texture3D;
	}
}