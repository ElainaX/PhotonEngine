#pragma once 
#include "Shader.h"
#include "ShaderMacros.h"
#include "Macro.h"

#include <unordered_map>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>

namespace photon 
{

	class TestShader : public Shader 
	{
	public:
		TestShader(const std::wstring& filepath)
			: Shader(filepath){}

		struct ShaderBlob
		{
			Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
			Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
		};

		ShaderBlob* Compile(const std::vector<MacroInfo>& macros)
		{
			if(m_Macros.IsVariantLoaded(macros))
			{
				return &m_VariantToShaderBlob[m_Macros.VectorToVariantString(macros)];
			}

			const D3D_SHADER_MACRO* pMacro = nullptr;
			ID3DInclude* pInclude = D3D_COMPILE_STANDARD_FILE_INCLUDE;

			LPCSTR pEntryPointVS = "VS";
			LPCSTR pTargetVS = "vs_5_0";


			LPCSTR pEntryPointPS = "PS";
			LPCSTR pTargetPS = "ps_5_0";

			ShaderBlob blob;

			std::string variant = m_Macros.SetVariantLoaded(macros);
			
			std::vector<D3D_SHADER_MACRO> d3dMacros = m_Macros.GetD3DMacroInfo(variant);
			if(!variant.empty())
			{
				pMacro = d3dMacros.data();
			}


			ID3DBlob* errorBlob = nullptr;
			HRESULT result;

			result = D3DCompileFromFile(sourceFilepath.c_str(), pMacro, pInclude,
				pEntryPointVS, pTargetVS, GetFlag1(), 0, &blob.vsBlob, &errorBlob);
			if(FAILED(result))
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
				assert(0);
			}

			result = D3DCompileFromFile(sourceFilepath.c_str(), pMacro, pInclude,
				pEntryPointPS, pTargetPS, GetFlag1(), 0, &blob.psBlob, &errorBlob);
			if (FAILED(result))
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
				assert(0);
			}

			m_VariantToShaderBlob.insert({ variant, blob });
			return &m_VariantToShaderBlob[variant];
		}

	private:



	private:
		ShaderMacros m_Macros;
		std::unordered_map<std::string, ShaderBlob> m_VariantToShaderBlob;
	};

}