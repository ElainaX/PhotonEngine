#pragma once 
#include "ConstantBufferParameter.h"
#include "SamplerParameter.h"
#include "TextureParameter.h"
#include "UAVParameter.h"

#include <vector>
#include <d3d12.h>
#include <unordered_map>
#include <string>

namespace photon 
{


	struct RootParameter
	{
		RootParameter(ConstantBufferParameter p)
			: cbufferParam(p), type(ShaderParameterType::ConstantBuffer), shaderVisibility(D3D12_SHADER_VISIBILITY_ALL){}
		RootParameter(SamplerParameter p)
			: samplerParam(p), type(ShaderParameterType::Sampler), shaderVisibility(D3D12_SHADER_VISIBILITY_ALL){}
		RootParameter(TextureParameter p)
			: textureParam(p), type(ShaderParameterType::Texture), shaderVisibility(D3D12_SHADER_VISIBILITY_ALL) {}
		RootParameter(UAVParameter p)
			: uavParam(p), type(ShaderParameterType::Uav), shaderVisibility(D3D12_SHADER_VISIBILITY_ALL) {}

		D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType() const
		{
			return (D3D12_DESCRIPTOR_RANGE_TYPE)(type);
		}

		union 
		{
			ConstantBufferParameter cbufferParam;
			SamplerParameter samplerParam;
			TextureParameter textureParam;
			UAVParameter uavParam;
			ShaderParameter shaderParam;
		};
		ShaderParameterType type;
		D3D12_SHADER_VISIBILITY shaderVisibility;
	};


	class RootSignature
	{
	public:
		void PushAsDescriptorTable(const RootParameter& p, UINT paramterNum);
		void PushAsRootDescriptor(const RootParameter& p);
		void PushAsRootConstants(const RootParameter& p, UINT totalNum32BitsValues);

		std::string TableParameterToString(const RootParameter& p);
		std::string DescriptorParameterToString(const RootParameter& p);
		std::string ConstantsParameterToString(const RootParameter& p);

		int GetTableParameterIndex(const RootParameter& p);
		int GetDescriptorParameterIndex(const RootParameter& p);
		int GetConstantsParameterIndex(const RootParameter& p);

		std::vector<D3D12_ROOT_PARAMETER> GetRootParameters() const 
		{
			return m_RootParameters;
		}

	private:
		static std::string s_TableString;
		static std::string s_DescriptorString;
		static std::string s_ConstantsString;

		int m_SignatureSize = 0;
		std::unordered_map<std::string, int> m_RootParameterStringToSignatureIndex;
		std::vector<D3D12_ROOT_PARAMETER> m_RootParameters;
		std::vector<D3D12_DESCRIPTOR_RANGE> m_Ranges;
	};
}