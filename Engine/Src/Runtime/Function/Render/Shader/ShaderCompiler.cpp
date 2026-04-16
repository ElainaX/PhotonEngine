#include "ShaderCompiler.h"

#include <d3dcompiler.h>
#include <wrl.h>

#include "Macro.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace photon
{
	CompiledShaderBytecode ShaderCompiler::Compile(const ShaderStageAsset& asset) const
	{
		CompiledShaderBytecode result;
		result.stage = asset.stage;
		result.entryPoint = asset.entryPoint;
		result.targetProfile = asset.targetProfile;

		std::vector<D3D_SHADER_MACRO> macros;
		macros.reserve(asset.macros.size() + 1);
		for (const auto& m : asset.macros)
		{
			D3D_SHADER_MACRO def = {};
			def.Name = m.macroDefine.c_str();
			def.Definition = m.value.empty() ? nullptr : m.value.c_str();
			macros.push_back(def);
		}
		macros.push_back({ nullptr, nullptr });

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		if (asset.enableDebugInfo)
			flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		else
			flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
		if (asset.treatWarningAsErrors)
			flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3DCompileFromFile(
			asset.sourcePath.c_str(),
			asset.macros.empty() ? nullptr : macros.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			asset.entryPoint.c_str(),
			asset.targetProfile.c_str(),
			flags,
			0,
			shaderBlob.GetAddressOf(),
			errorBlob.GetAddressOf());

		if (FAILED(hr) || !shaderBlob)
		{
			std::string errorMsg;
			if (errorBlob && errorBlob->GetBufferPointer())
			{
				errorMsg.assign(
					static_cast<const char*>(errorBlob->GetBufferPointer()),
					errorBlob->GetBufferSize());
			}
			else
			{
				errorMsg = "No compiler error blob.";
			}

			LOG_ERROR(
				"D3DCompileFromFile failed.\n"
				"  File: {}\n"
				"  Entry: {}\n"
				"  Profile: {}\n"
				"  HRESULT: 0x{:08X}\n"
				"  Message:\n{}",
				asset.sourcePath.string(),
				asset.entryPoint,
				asset.targetProfile,
				static_cast<unsigned int>(hr),
				errorMsg);

			return result;
		}

		result.data.resize(shaderBlob->GetBufferSize());
		std::memcpy(result.data.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
		return result;
	}
}
