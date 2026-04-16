#pragma once

#include <string>
#include <array>
#include <vector>

#include "ShaderEnum.h"
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/RenderObject/AssetMeta.h"
#include "ShaderMacros.h"
#include "ShaderParameter/ShaderConstantBuffer.h"

namespace photon 
{

	struct ShaderStageAsset
	{
		AssetMeta meta;

		ShaderStage stage = ShaderStage::VS;
		std::filesystem::path sourcePath;

		std::string entryPoint;
		std::string targetProfile;

		std::vector<MacroInfo> macros;

		bool enableDebugInfo = false;
		bool treatWarningAsErrors = false;
	};

	struct ShaderStageReference
	{
		bool valid = false;
		Guid stageAssetGuid;
	};

	struct ShaderParameterDesc
	{
		std::string name;
		ShaderParameterType type = ShaderParameterType::Cbv;
		ShaderParameterScope scope = ShaderParameterScope::Material;


		uint32_t bindPoint = 0;
		uint32_t registerSpace = 0;
		uint32_t arraySize = 1;
	};

	struct ShaderProgramAsset
	{
		AssetMeta meta;

		// 每个Stage是否存在，由Valid控制
		std::array<ShaderStageReference, static_cast<size_t>(ShaderStage::Count)> stages = {};

		// 资源级参数
		std::vector<ShaderParameterDesc> parameters;

		// 常量缓冲内部布局
		std::vector<ShaderConstantBufferDesc> constantBuffers;

		bool HasStage(ShaderStage stage) const
		{
			return stages[ToIndex(stage)].valid;
		}

		const ShaderStageReference* TryGetStage(ShaderStage stage) const
		{
			const auto& s = stages[ToIndex(stage)];
			return s.valid ? &s : nullptr;
		}

		void SetStage(ShaderStage stage, Guid guid)
		{
			auto& s = stages[ToIndex(stage)];
			s.valid = true;
			s.stageAssetGuid = guid;
		}

		void ClearStage(ShaderStage stage)
		{
			stages[ToIndex(stage)] = {};
		}
	};

	struct CompiledShaderBytecode
	{
		ShaderStage stage = ShaderStage::VS;

		std::string entryPoint;
		std::string targetProfile;

		std::vector<std::byte> data;

		bool Empty() const { return data.empty(); }
	};

	struct ShaderStageBytecodeSlot
	{
		bool valid = false;
		CompiledShaderBytecode bytecode;
	};

	struct NativeShaderBytecodeView
	{
		const void* ptr = nullptr;
		size_t size = 0;
	};

	struct ShaderProgramRenderResource
	{
		ShaderHandle handle;
		Guid assetGuid;

		std::array<ShaderStageBytecodeSlot, static_cast<size_t>(ShaderStage::Count)> stages = {};

		RootSignatureHandle rootSignature;

		std::vector<ShaderParameterDesc> parameters;
		std::vector<ShaderConstantBufferDesc> constantBuffers;

		bool HasStage(ShaderStage stage) const
		{
			return stages[ToIndex(stage)].valid;
		}

		const CompiledShaderBytecode* TryGetStageBytecode(ShaderStage stage) const
		{
			const auto& s = stages[ToIndex(stage)];
			return s.valid ? &s.bytecode : nullptr;
		}
	};

	inline ShaderProgramKind DeduceShaderProgramKind(const ShaderProgramAsset& asset)
	{
		if (asset.HasStage(ShaderStage::CS))
			return ShaderProgramKind::Compute;

		if (asset.HasStage(ShaderStage::MS) || asset.HasStage(ShaderStage::AS))
			return ShaderProgramKind::Mesh;

		return ShaderProgramKind::Graphics;
	}

	inline NativeShaderBytecodeView GetNativeBytecodeView(const CompiledShaderBytecode& bc)
	{
		NativeShaderBytecodeView view;
		view.ptr = bc.data.empty() ? nullptr : bc.data.data();
		view.size = bc.data.size();
		return view;
	}

	
}
