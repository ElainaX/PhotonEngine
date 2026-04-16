#pragma once
#include <cstdint>

namespace photon
{
	enum class ShaderStage : uint8_t
	{
		VS = 0,
		PS,
		CS,
		GS,
		HS,
		DS,
		AS,
		MS,
		Count
	};

	enum class ShaderParameterType : uint8_t
	{
		Cbv, // b
		Srv, // t
		Uav, // u
		Sampler, // s
	};

	enum class ShaderParameterScope : uint8_t
	{
		Pass,
		Material,
		Object,
		Instance,
		Frame,
		Global
	};

	enum  class ShaderValueType : uint8_t
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		UInt,
		Bool
	};


	enum class ShaderProgramKind : uint8_t
	{
		Graphics,
		Compute,
		Mesh
	};

	constexpr uint32_t ToIndex(ShaderStage stage)
	{
		return static_cast<uint32_t>(stage);
	}

	constexpr  uint32_t ShaderStageCount()
	{
		return static_cast<uint32_t>(ShaderStage::Count);
	}

	inline const char* ToString(ShaderStage stage)
	{
		switch (stage)
		{
		case ShaderStage::VS: return "VS";
		case ShaderStage::PS: return "PS";
		case ShaderStage::CS: return "CS";
		case ShaderStage::GS: return "GS";
		case ShaderStage::HS: return "HS";
		case ShaderStage::DS: return "DS";
		case ShaderStage::AS: return "AS";
		case ShaderStage::MS: return "MS";
		default: return "Unknown";
		}
	}

}
