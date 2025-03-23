#pragma once

#include "ShaderParameter/RootSignature.h"
#include "ShaderMacros.h"

#include <string>
#include <vector>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <wrl.h>

namespace photon 
{

	struct ShaderBlob
	{
		virtual D3D12_SHADER_BYTECODE GetVSShaderByteCode()
		{
			return D3D12_SHADER_BYTECODE{};
		}
		virtual D3D12_SHADER_BYTECODE GetPSShaderByteCode()
		{
			return D3D12_SHADER_BYTECODE{};
		}
		virtual D3D12_SHADER_BYTECODE GetDSShaderByteCode()
		{
			return D3D12_SHADER_BYTECODE{};
		}
		virtual D3D12_SHADER_BYTECODE GetHSShaderByteCode()
		{
			return D3D12_SHADER_BYTECODE{};
		}
		virtual D3D12_SHADER_BYTECODE GetGSShaderByteCode()
		{
			return D3D12_SHADER_BYTECODE{};
		}
		virtual D3D12_SHADER_BYTECODE GetCSShaderByteCode()
		{
			return D3D12_SHADER_BYTECODE{};
		}
	};

	class Shader
	{
	public:
		Shader(const std::wstring& filepath)
			: sourceFilepath(filepath) {
		}

		virtual void InitializeRootSignature() = 0;
		virtual Microsoft::WRL::ComPtr<ID3DBlob> GetDXSerializedRootSignatureBlob(int samplerCount = 0, const D3D12_STATIC_SAMPLER_DESC* samplerDesc = nullptr,
			D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT) = 0;
		virtual std::vector<D3D12_INPUT_ELEMENT_DESC>& GetShaderInputLayout() = 0;

		RootSignature* GetPhotonRootSignature(){
			return &m_Signature;
		}

		ShaderMacros* GetMacros(){
			return &m_Macros;
		}

		virtual ShaderBlob* Compile(const std::vector<MacroInfo>& macros) = 0;

		std::wstring sourceFilepath;
		
	protected:
		UINT GetFlag1()
		{
#if defined(DEBUG) || defined(_DEBUG)
			return D3DCOMPILE_DEBUG;
#else
			return D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		}

	protected:
		RootSignature m_Signature;
		ShaderMacros m_Macros;
	};
}