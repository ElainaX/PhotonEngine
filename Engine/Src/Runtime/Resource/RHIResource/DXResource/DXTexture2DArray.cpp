#include "DXTexture2DArray.h"

namespace photon 
{


	bool DXTexture2DArray::Initialize(const DXTexture2DArrayDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes,
	                                  const std::string& _name)
	{
		DXTextureDesc texdesc = ToDXTextureDesc(desc);
		return DXTexture::Initialize(texdesc, GetResourceDim(), _gpuRes, _name);
	}

	DXTextureDesc DXTexture2DArray::ToDXTextureDesc(const DXTexture2DArrayDesc& desc)
	{
		DXTextureDesc texDesc = {};
		texDesc.width = desc.width;
		texDesc.height = desc.height;
		texDesc.depthOrArraySize = desc.arraySize;
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

	D3D12_RESOURCE_DESC DXTexture2DArray::ToDxDesc(const DXTexture2DArrayDesc& desc)
	{
		return DXTexture::ToDxDesc(ToDXTextureDesc(desc));
	}

	D3D12_RESOURCE_FLAGS DXTexture2DArray::GetResourceFlags(const DXTexture2DArrayDesc& desc)
	{
		return desc.flag;
	}

	D3D12_RESOURCE_STATES DXTexture2DArray::GetInitialResourceState(const DXTexture2DArrayDesc& desc)
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

	DXResourceDimension DXTexture2DArray::GetResourceDim()
	{
		return DXResourceDimension::Texture2DArray;
	}

}