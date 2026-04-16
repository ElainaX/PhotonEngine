#pragma once 
#include "Function/Render/ResourceHandle.h"

namespace photon
{
	struct ComputePipelineKey
	{
		ShaderHandle shaderProgram;
		RootSignatureHandle rootSignature;
	};

	inline bool operator==(const ComputePipelineKey& a, const ComputePipelineKey& b)
	{
		return a.shaderProgram == b.shaderProgram && a.rootSignature == b.rootSignature;
	}

	struct ComputePipelineKeyHasher
	{
		size_t operator()(const ComputePipelineKey& key) const noexcept
		{
			size_t seed = 0;
			auto hashCombine = [](size_t& s, size_t v)
				{
					s ^= v + 0x9e3779b97f4a7c15ull + (s << 6) + (s >> 2);
				};
			hashCombine(seed, std::hash<uint64_t>{}(key.shaderProgram.handle.index));
			hashCombine(seed, std::hash<uint32_t>{}(key.shaderProgram.handle.generation));
			hashCombine(seed, std::hash<uint64_t>{}(key.rootSignature.handle.index));
			hashCombine(seed, std::hash<uint32_t>{}(key.rootSignature.handle.generation));
			return seed;
		}
	};
}
