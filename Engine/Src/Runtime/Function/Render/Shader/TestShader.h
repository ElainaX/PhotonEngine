#pragma once 
#include "Shader.h"
#include "Macro.h"

#include <unordered_map>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>


#include "../DX12RHI/DX12Resource/VertexType.h"
#include "Macro.h"

namespace photon 
{


	struct TestShaderBlob : public ShaderBlob
	{
		virtual D3D12_SHADER_BYTECODE GetVSShaderByteCode() override final 
		{
			return D3D12_SHADER_BYTECODE{ reinterpret_cast<BYTE*>(vsBlob->GetBufferPointer()), vsBlob->GetBufferSize() };
		}
		virtual D3D12_SHADER_BYTECODE GetPSShaderByteCode() override final
		{
			return D3D12_SHADER_BYTECODE{ reinterpret_cast<BYTE*>(psBlob->GetBufferPointer()), psBlob->GetBufferSize() };
		}

		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	};

	class TestShader : public Shader 
	{
	public:
		TestShader(const std::wstring& filepath)
			: Shader(filepath){
			InitializeRootSignature();
		}


		virtual ShaderBlob* Compile(const std::vector<MacroInfo>& macros) override
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

			TestShaderBlob blob;

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

		void InitializeRootSignature() override
		{
			m_Signature.PushAsDescriptorTable(ConstantBufferParameter(1), 1);
			//m_Signature.PushAsRootDescriptor(ConstantBufferParameter(1));
		}


		Microsoft::WRL::ComPtr<ID3DBlob> GetDXSerializedRootSignatureBlob(int samplerCount = 0, const D3D12_STATIC_SAMPLER_DESC* samplerDesc = nullptr,
			D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT) override
		{
			using namespace Microsoft::WRL;
			std::vector<D3D12_ROOT_PARAMETER> parameters = m_Signature.GetRootParameters();
			D3D12_ROOT_SIGNATURE_DESC rootSigDesc((UINT)parameters.size(), parameters.data(), samplerCount, samplerDesc, flag);
			ComPtr<ID3DBlob> serializedRootSig = nullptr;
			ComPtr<ID3DBlob> errorBlob = nullptr;
			DX_LogIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob));
			return serializedRootSig;
		}


		std::vector<D3D12_INPUT_ELEMENT_DESC>& GetShaderInputLayout() override
		{
			return VertexSimple::GetInputLayout();
		}

	private:

		std::unordered_map<std::string, TestShaderBlob> m_VariantToShaderBlob;
	};

}