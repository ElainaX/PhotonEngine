#pragma once 

#include <wrl.h>
#include <d3d12.h>

#include "Resource/ResourceType.h"

namespace photon
{
	struct Texture2DDesc
	{
		std::string name = "Texture2D";
		unsigned int width;
		unsigned int height;
		unsigned int maxMipLevel = 1; // 从1开始
		DXGI_FORMAT format;
		unsigned int sampleCount = 1;
		unsigned int sampleQuality = 0;
		D3D12_RESOURCE_FLAGS flag;

		ResourceHeapProperties heapProp = ResourceHeapProperties::Static;
	};

	class Texture2D : public Resource
	{
	public:
		Texture2D() = default;
		Texture2D(Texture2DDesc desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource);
		Texture2D(D3D12_RESOURCE_DESC desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource);

	protected:

	};
}