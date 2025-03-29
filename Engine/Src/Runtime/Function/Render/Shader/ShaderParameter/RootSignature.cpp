#include "RootSignature.h"

#include <format>

namespace photon 
{

	void RootSignature::PushAsDescriptorTable(const RootParameter& p, UINT paramterNum)
	{
		UINT rangeNum = 1;
		D3D12_DESCRIPTOR_RANGE range;
		range.RangeType = p.GetDescriptorRangeType();
		range.NumDescriptors = paramterNum;
		range.BaseShaderRegister = p.shaderParam.slot;
		range.RegisterSpace = p.shaderParam.space;
		range.OffsetInDescriptorsFromTableStart = 0;
		m_Ranges.push_back(range);

		D3D12_ROOT_DESCRIPTOR_TABLE table;
		table.NumDescriptorRanges = rangeNum;
		table.pDescriptorRanges = &m_Ranges.back();


		D3D12_ROOT_PARAMETER parameter;
		parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		parameter.DescriptorTable = table;
		parameter.ShaderVisibility = p.shaderVisibility;

		m_RootParameters.push_back(parameter);

		std::string hashStr = TableParameterToString(p);
		if(m_RootParameterStringToSignatureIndex.contains(hashStr))
		{
			throw;
		}
		m_RootParameterStringToSignatureIndex[hashStr] = m_SignatureSize++;
	}

	void RootSignature::PushAsRootDescriptor(const RootParameter& p)
	{
		D3D12_ROOT_DESCRIPTOR descriptor;
		descriptor.ShaderRegister = p.shaderParam.slot;
		descriptor.RegisterSpace = p.shaderParam.space;

		D3D12_ROOT_PARAMETER_TYPE paramType;
		switch (p.type)
		{
		case ShaderParameterType::ConstantBuffer:
			paramType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			break;
		case ShaderParameterType::Texture:
			paramType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			break;
		case ShaderParameterType::Uav:
			paramType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			break;
		}


		D3D12_ROOT_PARAMETER parameter;
		parameter.ParameterType = paramType;
		parameter.Descriptor = descriptor;
		parameter.ShaderVisibility = p.shaderVisibility;

		m_RootParameters.push_back(parameter);
		std::string hashStr = DescriptorParameterToString(p);
		if (m_RootParameterStringToSignatureIndex.contains(hashStr))
		{
			throw;
		}
		m_RootParameterStringToSignatureIndex[hashStr] = m_SignatureSize++;
	}

	void RootSignature::PushAsRootConstants(const RootParameter& p, UINT totalNum32BitsValues)
	{
		D3D12_ROOT_CONSTANTS constants;
		constants.ShaderRegister = p.shaderParam.slot;
		constants.RegisterSpace = p.shaderParam.space;
		constants.Num32BitValues = totalNum32BitsValues;

		D3D12_ROOT_PARAMETER parameter;
		parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		parameter.Constants = constants;
		parameter.ShaderVisibility = p.shaderVisibility;

		m_RootParameters.push_back(parameter);
		std::string hashStr = ConstantsParameterToString(p);
		if (m_RootParameterStringToSignatureIndex.contains(hashStr))
		{
			throw;
		}
		m_RootParameterStringToSignatureIndex[hashStr] = m_SignatureSize++;
	}

	std::string RootSignature::TableParameterToString(const RootParameter& p)
	{
		std::string ret = std::format("{}:space{},register{},type{}", 
			s_TableString, p.shaderParam.space, p.shaderParam.slot, (int)p.type);
		return ret;
	}


	std::string RootSignature::DescriptorParameterToString(const RootParameter& p)
	{
		std::string ret = std::format("{}:space{},register{},type{}", 
			s_DescriptorString, p.shaderParam.space, p.shaderParam.slot, (int)p.type);
		return ret;
	}


	std::string RootSignature::ConstantsParameterToString(const RootParameter& p)
	{
		std::string ret = std::format("{}:space{},register{}", 
			s_ConstantsString, p.shaderParam.space, p.shaderParam.slot);
		return ret;
	}

	int RootSignature::GetTableParameterIndex(const RootParameter& p)
	{
		return m_RootParameterStringToSignatureIndex[TableParameterToString(p)];
	}

	int RootSignature::GetDescriptorParameterIndex(const RootParameter& p)
	{
		return m_RootParameterStringToSignatureIndex[DescriptorParameterToString(p)];
	}

	int RootSignature::GetConstantsParameterIndex(const RootParameter& p)
	{
		return m_RootParameterStringToSignatureIndex[ConstantsParameterToString(p)];
	}

	std::string RootSignature::s_TableString = "table";
	std::string RootSignature::s_DescriptorString = "descriptor";
	std::string RootSignature::s_ConstantsString = "constants";

}