#pragma once
#include "Function/Render/ResourceLoader/ResourceLoader.h"

namespace photon 
{
	class ShaderFactory
	{
	public:
		static ShaderProgramLoadDesc BuildPbrShaderDesc();
	};
}
