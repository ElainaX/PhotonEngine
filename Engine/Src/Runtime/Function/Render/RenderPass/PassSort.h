#pragma once

#include <cstdint>
#include <algorithm>

namespace photon
{
	enum class PassSortMode : uint8_t
	{
		Opaque,
		Transparent,
		DepthOnly,
		Shadow,
	};

	inline uint32_t FloatDepthToSortableUint(float z)
	{
		z = std::clamp(z, 0.0f, 1.0f);
		return static_cast<uint32_t>(z * 4294967295.0f);
	}

	inline uint64_t BuildOpaqueSortKey(
		uint32_t pipelineKey,
		uint32_t materialKey,
		uint16_t meshKey,
		uint16_t depthKey)
	{
		// 高位优先状态，低位再放深度
		return (static_cast<uint64_t>(pipelineKey) << 32)
			| (static_cast<uint64_t>(materialKey) << 16)
			| (static_cast<uint64_t>(meshKey) << 0)
			| (static_cast<uint64_t>(depthKey) & 0xFFFFull);
	}

	inline uint64_t BuildTransparentSortKey(
		uint32_t depthBackToFront,
		uint16_t pipelineKey,
		uint16_t materialKey)
	{
		// 透明优先深度正确性：远到近
		return (static_cast<uint64_t>(depthBackToFront) << 32)
			| (static_cast<uint64_t>(pipelineKey) << 16)
			| static_cast<uint64_t>(materialKey);
	}
}

