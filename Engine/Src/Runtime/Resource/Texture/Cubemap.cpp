#include "Cubemap.h"

namespace photon 
{

	Cubemap::Cubemap(CubemapDesc desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource)
	{
		dxDesc = ToDxDesc(desc);
		heapProp = desc.heapProp;


		cubemapTextures = desc.cubemapTextures;
		gpuResource = _gpuResource;
		clearValue = desc.clearValue;
	}

	Cubemap::Cubemap(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, 
		std::array<std::shared_ptr<Texture2D>, 6> cubemaptextures, Vector4 _clearValue /*= { 1.0f, 1.0f, 1.0f, 1.0f }*/)
	{
		dxDesc = desc;
		heapProp = prop;

		cubemapTextures = cubemaptextures;
		gpuResource = _gpuResource;
		clearValue = _clearValue;
	}

	D3D12_RESOURCE_DESC Cubemap::ToDxDesc(CubemapDesc desc)
	{
		D3D12_RESOURCE_DESC dxDesc;
		dxDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dxDesc.Format = desc.format;
		dxDesc.MipLevels = desc.maxMipLevels;
		dxDesc.Alignment = 0;
		dxDesc.DepthOrArraySize = desc.depthOrArraySize;
		dxDesc.Flags = desc.flag;
		dxDesc.Width = desc.width;
		dxDesc.Height = desc.height;
		dxDesc.SampleDesc.Count = desc.sampleCount;
		dxDesc.SampleDesc.Quality = desc.sampleQuality;
		dxDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		return dxDesc;
	}

	photon::CubemapDesc Cubemap::ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp, std::array<std::shared_ptr<Texture2D>, 6> cubemapTextures, Vector4 _clearValue /*= { 1.0f, 1.0f, 1.0f, 1.0f }*/)
	{
		CubemapDesc desc;
		desc.width = dxDesc.Width;
		desc.height = dxDesc.Height;
		desc.depthOrArraySize = dxDesc.DepthOrArraySize;
		desc.format = dxDesc.Format;
		desc.maxMipLevels = dxDesc.MipLevels;
		desc.sampleCount = dxDesc.SampleDesc.Count;
		desc.sampleQuality = dxDesc.SampleDesc.Quality;
		desc.flag = dxDesc.Flags;
		desc.heapProp = heapProp;
		desc.clearValue = _clearValue;
		desc.cubemapTextures = cubemapTextures;
		return desc;
	}

}