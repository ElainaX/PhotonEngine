#pragma once

#include "ShaderParameter.h"

namespace photon 
{
	class SamplerParameter : public ShaderParameter
	{
	public:
		SamplerParameter(unsigned int _slot, unsigned int _space = 0)
			: ShaderParameter(_slot, ShaderParameterType::Sampler, _space) {
		}
	};
}