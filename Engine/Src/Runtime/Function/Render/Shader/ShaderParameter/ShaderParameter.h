#pragma once

#include <d3d12.h>

namespace photon
{
	enum class ShaderParameterType
	{
		ConstantBuffer = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
		Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
		Texture = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		Uav = D3D12_DESCRIPTOR_RANGE_TYPE_UAV
	};

	class ShaderParameter
	{
	public:
		ShaderParameter(unsigned int _slot, ShaderParameterType _type, unsigned int _space = 0)
			: slot(_slot), space(_space), type(_type){}

		D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType() const
		{
			return (D3D12_DESCRIPTOR_RANGE_TYPE)(type);
		}

	public:
		unsigned int slot = 0;
		unsigned int space = 0;
		ShaderParameterType type;
	};
}