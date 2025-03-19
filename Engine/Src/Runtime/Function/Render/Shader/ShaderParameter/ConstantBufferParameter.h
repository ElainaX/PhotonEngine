#pragma once

#include "ShaderParameter.h"

namespace photon
{
	class ConstantBufferParameter : public ShaderParameter
	{
	public:
		ConstantBufferParameter(unsigned int _slot, unsigned int _space = 0)
			: ShaderParameter(_slot, ShaderParameterType::ConstantBuffer, _space){}
	};
}