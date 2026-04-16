#pragma once 

#include <vector>

#include "Function/Render/Shader/Shader.h"

namespace photon 
{
	struct RootParameterDesc
	{
		ShaderParameterType type = ShaderParameterType::Cbv;
		uint32_t bindPoint = 0;
		uint32_t registerSpace = 0;
		uint32_t numDescriptors = 1;

		ShaderParameterScope scope = ShaderParameterScope::Material;

		bool operator==(const RootParameterDesc& rhs) const = default;
	};

	inline void HashCombineRootSig(size_t& seed, size_t value)
	{
		seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2);
	}

	inline size_t HashRootParameterDesc(const RootParameterDesc& desc)
	{
		size_t seed = 0;
		HashCombineRootSig(seed, std::hash<uint32_t>{}(static_cast<uint32_t>(desc.type)));
		HashCombineRootSig(seed, std::hash<uint32_t>{}(desc.bindPoint));
		HashCombineRootSig(seed, std::hash<uint32_t>{}(desc.registerSpace));
		HashCombineRootSig(seed, std::hash<uint32_t>{}(desc.numDescriptors));
		HashCombineRootSig(seed, std::hash<uint32_t>{}(static_cast<uint32_t>(desc.scope)));
		return seed;
	}

	struct RootSignatureDesc
	{
		bool allowInputAssembler = true;
		std::vector<RootParameterDesc> parameters;

		bool operator==(const RootSignatureDesc& rhs) const
		{
			return allowInputAssembler == rhs.allowInputAssembler && parameters == rhs.parameters;
		}
	};

	inline size_t HashRootSignatureDesc(const RootSignatureDesc& desc)
	{
		size_t seed = 0;
		HashCombineRootSig(seed, std::hash<bool>{}(desc.allowInputAssembler));
		for (const auto& p : desc.parameters)
		{
			HashCombineRootSig(seed, HashRootParameterDesc(p));
		}
		return seed;
	}


	inline RootSignatureDesc BuildRootSignatureDesc(const ShaderProgramAsset& asset)
	{
		RootSignatureDesc rs;
		rs.allowInputAssembler = true;

		for (const auto& b : asset.parameters)
		{
			RootParameterDesc p;
			p.type = b.type;
			p.bindPoint = b.bindPoint;
			p.registerSpace = b.registerSpace;
			p.numDescriptors = b.arraySize;
			p.scope = b.scope;
			rs.parameters.push_back(p);

		}

		return rs;
	}



}
