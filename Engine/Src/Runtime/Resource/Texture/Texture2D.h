#pragma once 

#include <wrl.h>
#include <d3d12.h>

#include "Resource/ResourceType.h"
#include "Core/Math/Vector4.h"

namespace photon
{

	struct Texture2DDesc
	{
		unsigned int width;
		unsigned int height;
		unsigned int maxMipLevel = 1; // 从1开始
		DXGI_FORMAT format;
		unsigned int sampleCount = 1;
		unsigned int sampleQuality = 0;
		D3D12_RESOURCE_FLAGS flag;

		ResourceHeapProperties heapProp = ResourceHeapProperties::Default;
		Vector4 clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };

		Microsoft::WRL::ComPtr<ID3DBlob> cpuResource = nullptr;
	};

	class Texture2D : public Resource
	{
	public:
		Texture2D() = default;
		Texture2D(Texture2DDesc desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource);
		Texture2D(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop, 
			Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, Microsoft::WRL::ComPtr<ID3DBlob> _cpuResource, Vector4 _clearValue = { 1.0f, 1.0f, 1.0f, 1.0f });
		static D3D12_RESOURCE_DESC ToDxDesc (Texture2DDesc desc);
		static Texture2DDesc ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp,
			Vector4 _clearValue = { 1.0f, 1.0f, 1.0f, 1.0f });
		
		Vector4 clearValue;
	};
}