#pragma once

#include "ShaderParameter.h"

namespace photon
{
	class TextureParameter : public ShaderParameter
	{
	public:
		TextureParameter(unsigned int _slot, unsigned int _space = 0)
			: ShaderParameter(_slot, ShaderParameterType::Texture, _space) {
		}
	};
}