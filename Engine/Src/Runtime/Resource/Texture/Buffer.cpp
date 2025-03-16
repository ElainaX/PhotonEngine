#include "Buffer.h"

namespace photon 
{

	Buffer::Buffer(BufferDesc bufferDesc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, Microsoft::WRL::ComPtr<ID3DBlob> _cpuResource /*= nullptr*/)
	{
		dxDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		dxDesc.Alignment = 0;
		dxDesc.Width = bufferDesc.bufferSizeInBytes;
		dxDesc.Height = 1;
		dxDesc.DepthOrArraySize = 1;
		dxDesc.MipLevels = 1;
		dxDesc.Format = DXGI_FORMAT_UNKNOWN;
		dxDesc.SampleDesc.Count = 1;
		dxDesc.SampleDesc.Quality = 0;
		dxDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		dxDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		heapProp = bufferDesc.heapProp;
		gpuResource = _gpuResource;
		cpuResource = _cpuResource;
	}

	Buffer::Buffer(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, Microsoft::WRL::ComPtr<ID3DBlob> _cpuResource /*= nullptr*/)
	{
		dxDesc = desc;
		heapProp = prop;
		gpuResource = _gpuResource;
		cpuResource = _cpuResource;
	}

	D3D12_RESOURCE_DESC Buffer::ToDxDesc(BufferDesc desc)
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
		dxDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		return dxDesc;
	}

	photon::BufferDesc Buffer::ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp)
	{
		BufferDesc desc;
		desc.bufferSizeInBytes = dxDesc.Width;
		desc.heapProp = heapProp;
		return desc;
	}

}