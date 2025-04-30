#pragma once
#include "Resource/ResourceType.h"
#include "Resource/Texture/Texture2D.h"
#include "Core/Math/Vector4.h"

#include <array>
#include <memory>
#include <d3dcompiler.h>


namespace photon
{
	struct Texture2DArrayDesc
	{
		unsigned int width;
		unsigned int height;
		unsigned int depthOrArraySize = 1;
		unsigned int maxMipLevels = 1;
		DXGI_FORMAT format;
		unsigned int sampleCount = 1;
		unsigned int sampleQuality = 0;
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		ResourceHeapProperties heapProp = ResourceHeapProperties::Default;
		Vector4 clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::vector<std::shared_ptr<Texture2D>> textures;
	};

	class Texture2DArray : public Resource
	{
	public:
		Texture2DArray() = default;
		Texture2DArray(Texture2DArrayDesc desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource);
		Texture2DArray(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop,
			Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, const std::vector<std::shared_ptr<Texture2D>>& _textures, Vector4 _clearValue = { 1.0f, 1.0f, 1.0f, 1.0f });

		static D3D12_RESOURCE_DESC ToDxDesc(Texture2DArrayDesc desc);
		static Texture2DArrayDesc ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp,
			const std::vector<std::shared_ptr<Texture2D>>& _textures, Vector4 _clearValue = { 1.0f, 1.0f, 1.0f, 1.0f });

		Vector4 clearValue;
		std::vector<std::shared_ptr<Texture2D>> textures;
	};
}