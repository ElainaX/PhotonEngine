#include "DXBuffer.h"

namespace photon 
{
	bool DXBuffer::Initialize(const DXBufferDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes, const std::string& _name)
	{
		allowUav = desc.allowUav;
		return DXResource::Initialize(GetResourceDim(), _name, desc.heapProp, 
			GetInitialResourceState(desc), ToDxDesc(desc), nullptr, _gpuRes);
	}

	D3D12_RESOURCE_FLAGS DXBuffer::GetResourceFlags(const DXBufferDesc& desc)
	{
		if (desc.allowUav)
			return D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		return D3D12_RESOURCE_FLAG_NONE;
	}

	D3D12_RESOURCE_DESC DXBuffer::ToDxDesc(const DXBufferDesc& desc)
	{
		D3D12_RESOURCE_DESC dxDesc;
		dxDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		dxDesc.Alignment = 0;
		dxDesc.Width = desc.bufferSizeInBytes;
		dxDesc.Height = 1;
		dxDesc.DepthOrArraySize = 1;
		dxDesc.MipLevels = 1;
		dxDesc.Format = DXGI_FORMAT_UNKNOWN;
		dxDesc.SampleDesc.Count = 1;
		dxDesc.SampleDesc.Quality = 0;
		dxDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		dxDesc.Flags = GetResourceFlags(desc);
		return dxDesc;
	}

	D3D12_RESOURCE_STATES DXBuffer::GetInitialResourceState(const DXBufferDesc& desc)
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

	DXResourceDimension DXBuffer::GetResourceDim()
	{
		return DXResourceDimension::Buffer;
	}

	//photon::DXBufferDesc DXBuffer::ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, HeapProp heapProp)
	//{
	//	DXBufferDesc desc;
	//	desc.bufferSizeInBytes = dxDesc.Width;
	//	desc.heapProp = heapProp;
	//	return desc;
	//}

}