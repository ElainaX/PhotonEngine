#pragma once

#include "ShaderParameter.h"

namespace photon
{
	class UAVParameter : public ShaderParameter
	{
	public:
		UAVParameter(unsigned int _slot, unsigned int _space = 0)
			: ShaderParameter(_slot, ShaderParameterType::Uav, _space) {
		}
	};
}