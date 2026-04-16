#pragma once
#include <cstdint>
#include <compare>
#include <dxgiformat.h>

#include "d3dx12.h"
#include "DX12Define.h"

namespace photon
{


	// 此ViewKey适用于Texture的SRV，UAV，DSV，RTV等
	struct ViewKey
	{
		// ViewKey的类型划分
		ViewType type = ViewType::Unknown;
		ViewDimension dimension = ViewDimension::Unknown;

		// 存储的解释方式
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		uint32_t flags = ViewKeyFlag_None; // 这个应当被表示为ViewKeyFlag的组合

		// Srv 特有，需不需要重排列rgba通道
		uint32_t shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// Texture 子资源范围
		uint32_t mostDetailedMip = 0;
		uint32_t mipLevels = 1;
		uint32_t mipSlice = 0;
		uint32_t firstArraySlice = 0;
		uint32_t arraySize = 1;

		auto operator<=>(const ViewKey&) const = default;
	};

	struct ViewKeyHasher
	{
		size_t operator()(const ViewKey& key) const
		{
			size_t seed = 0;
			auto hashCombine = [](size_t& s, size_t v)
				{
					s ^= v + 0x9e3779b97f4a7c15ull + (s << 6) + (s >> 2);
				};
			hashCombine(seed, std::hash< uint32_t>{}(
				(static_cast<uint32_t>(key.type) << 16) | (static_cast<uint32_t>(key.dimension))));
			hashCombine(seed, std::hash<uint32_t>{}(static_cast<uint32_t>(key.format)));
			hashCombine(seed, std::hash<uint32_t>{}(key.flags));
			hashCombine(seed, std::hash<uint32_t>{}(key.shader4ComponentMapping));
			hashCombine(seed, std::hash<uint32_t>{}(key.mostDetailedMip));
			hashCombine(seed, std::hash<uint32_t>{}(key.mipLevels));
			hashCombine(seed, std::hash<uint32_t>{}(key.mipSlice));
			hashCombine(seed, std::hash<uint32_t>{}(key.firstArraySlice));
			hashCombine(seed, std::hash<uint32_t>{}(key.arraySize));
			return seed;
		}
	};

	struct CbvKey
	{
		uint64_t offsetInBytes = 0;
		uint32_t sizeInBytes = 0;

		auto operator<=>(const CbvKey&) const = default;
	};

	struct CbvKeyHasher
	{
		size_t operator()(const CbvKey& key) const noexcept
		{
			size_t seed = 0;
			auto hashCombine = [](size_t& s, size_t v)
				{
					s ^= v + 0x9e3779b97f4a7c15ull + (s << 6) + (s >> 2);
				};
			hashCombine(seed, std::hash<uint64_t>{}(key.offsetInBytes));
			hashCombine(seed, std::hash<uint32_t>{}(key.sizeInBytes));

			return seed;
		}
	};
}
