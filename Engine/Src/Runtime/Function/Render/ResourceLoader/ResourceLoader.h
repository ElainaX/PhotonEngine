#pragma once
#include "Function/Render/Shader/Shader.h"
#include "Function/Render/Shader/ShaderParameter/ShaderConstantBuffer.h"

namespace photon
{

	struct ShaderProgramLoadDesc
	{
		std::array<bool, static_cast<size_t>(ShaderStage::Count)> enableStages = {};
		std::array<std::string, static_cast<size_t>(ShaderStage::Count)> entryPoints = {};
		std::array<std::string, static_cast<size_t>(ShaderStage::Count)> targetProfiles = {};
		std::vector<ShaderParameterDesc> parameters;
		std::vector<ShaderConstantBufferDesc> constantBuffers;
		bool enableDebugInfo = false;
		bool treatWarningsAsErrors = false;

		static ShaderProgramLoadDesc GraphicsDefault();
		static ShaderProgramLoadDesc ComputeDefault();
	};

	//class ResourceLoader
	//{
	//public:
	//	ResourceLoader() = default;
	//	bool Initialize() { return true; }
	//	void Shutdown() {}

	//};
}
