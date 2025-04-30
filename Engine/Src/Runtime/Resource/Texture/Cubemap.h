#pragma once
#include "Resource/ResourceType.h"
#include "Resource/Texture/Texture2D.h"
#include "Core/Math/Vector4.h"

#include <array>
#include <memory>
#include <d3dcompiler.h>


namespace photon 
{
	struct CubemapDesc
	{
		unsigned int width;
		unsigned int height;
		unsigned int depthOrArraySize = 6;
		unsigned int maxMipLevels = 1;
		DXGI_FORMAT format;
		unsigned int sampleCount = 1;
		unsigned int sampleQuality = 0;
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		ResourceHeapProperties heapProp = ResourceHeapProperties::Default;
		Vector4 clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::array<std::shared_ptr<Texture2D>, 6> cubemapTextures;
	};

	class Cubemap : public Resource
	{
	public:
		Cubemap() = default;
		Cubemap(CubemapDesc desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource);
		Cubemap(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop,
			Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, std::array<std::shared_ptr<Texture2D>, 6> cubemapTextures, Vector4 _clearValue = { 1.0f, 1.0f, 1.0f, 1.0f });

		static D3D12_RESOURCE_DESC ToDxDesc(CubemapDesc desc);
		static CubemapDesc ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp,
			std::array<std::shared_ptr<Texture2D>, 6> cubemaptextures, Vector4 _clearValue = { 1.0f, 1.0f, 1.0f, 1.0f });

		Vector4 clearValue;
		std::array<std::shared_ptr<Texture2D>, 6> cubemapTextures;
	};
}