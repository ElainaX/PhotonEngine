#include "Texture2D.h"

namespace photon 
{


	Texture2D::Texture2D(Texture2DDesc desc, 
		Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, Microsoft::WRL::ComPtr<ID3DBlob> _cpuResource)
	{
		dxDesc = ToDxDesc(desc);
		heapProp = desc.heapProp;

		gpuResource = _gpuResource;
		cpuResource = _cpuResource;
	}



	Texture2D::Texture2D(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop, 
		Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, Microsoft::WRL::ComPtr<ID3DBlob> _cpuResource)
	{
		dxDesc = desc;

		gpuResource = _gpuResource;
		cpuResource = _cpuResource;
		heapProp = prop;
	}

	D3D12_RESOURCE_DESC Texture2D::ToDxDesc(Texture2DDesc desc)
	{
		D3D12_RESOURCE_DESC dxDesc;
		dxDesc.Format = desc.format;
		dxDesc.MipLevels = desc.maxMipLevel;
		dxDesc.Width = desc.width;
		dxDesc.Height = desc.height;
		dxDesc.Flags = desc.flag;
		dxDesc.Alignment = 0;
		dxDesc.DepthOrArraySize = 1;
		dxDesc.SampleDesc.Count = desc.sampleCount;
		dxDesc.SampleDesc.Quality = desc.sampleQuality;
		dxDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dxDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		return dxDesc;
	}

	photon::Texture2DDesc Texture2D::ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp)
	{
		Texture2DDesc desc;
		desc.format = dxDesc.Format;
		desc.maxMipLevel = dxDesc.MipLevels;
		desc.width = dxDesc.Width;
		desc.height = dxDesc.Height;
		desc.flag = dxDesc.Flags;
		desc.sampleCount = dxDesc.SampleDesc.Count;
		desc.sampleQuality = dxDesc.SampleDesc.Quality;

		desc.heapProp = heapProp;
		return desc;
	}

}