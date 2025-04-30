#include "Texture2DArray.h"

namespace photon 
{


	Texture2DArray::Texture2DArray(Texture2DArrayDesc desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource)
	{
		dxDesc = ToDxDesc(desc);
		heapProp = desc.heapProp;


		textures = desc.textures;
		gpuResource = _gpuResource;
		clearValue = desc.clearValue;
	}

	Texture2DArray::Texture2DArray(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, 
		const std::vector<std::shared_ptr<Texture2D>>& _textures, Vector4 _clearValue /*= { 1.0f, 1.0f, 1.0f, 1.0f }*/)
	{
		dxDesc = desc;
		heapProp = prop;

		textures = _textures;
		gpuResource = _gpuResource;
		clearValue = _clearValue;
	}

	D3D12_RESOURCE_DESC Texture2DArray::ToDxDesc(Texture2DArrayDesc desc)
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

	photon::Texture2DArrayDesc Texture2DArray::ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp, 
		const std::vector<std::shared_ptr<Texture2D>>& _textures, Vector4 _clearValue /*= { 1.0f, 1.0f, 1.0f, 1.0f }*/)
	{
		Texture2DArrayDesc desc;
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
		desc.textures = _textures;
		return desc;
	}

}