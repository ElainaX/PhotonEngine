#pragma once
#include "Shader.h"

namespace photon
{
	inline D3D12_SHADER_BYTECODE ToD3D12Bytecode(const CompiledShaderBytecode& bc)
	{
		D3D12_SHADER_BYTECODE ret = {};
		ret.pShaderBytecode = bc.data.empty() ? nullptr : bc.data.data();
		ret.BytecodeLength = bc.data.size();
		return ret;
	}

	class ShaderCompiler
	{
	public:
		CompiledShaderBytecode Compile(const ShaderStageAsset& asset) const;
	};
}

