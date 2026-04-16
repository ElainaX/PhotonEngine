#pragma once
#include <filesystem>
#include <array>

#include "AssetMeta.h"
#include "MaterialRenderState.h"
#include "Function/Render/ResourceTypes.h"
#include "Function/Render/Shader/ShaderParameter/ShaderConstantBuffer.h"


namespace photon
{
	enum class MaterialParamType : uint8_t
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		UInt,
		Bool
	};

	enum class PBRMaterialTextureUsage : uint8_t
	{
		BaseColor,
		Normal,
		Roughness,
		emissive,
		occlusion
	};

	//struct MaterialScalarParam
	//{
	//	std::string name;
	//	MaterialParamType type = MaterialParamType::Float4;
	//	std::array<float, 4> value = { 0, 0, 0, 0 };
	//};

	struct MaterialTextureBinding
	{
		std::string name; // BaseColor / Normal / ORM / Emissive ...
		Guid textureGuid; // 逻辑引用
	};


	struct MaterialAsset
	{
		AssetMeta meta;

		Guid shaderProgramAssetGuid;

		ConstantBlock matConstant = {.semantic = ShaderParameterScope::Material};
		std::vector<MaterialTextureBinding> textureBindings;

		// 仅保存材质语义上的渲染意图，不直接绑定具体 PSO。
		// 具体的 Blend / Raster / DepthStencil 由 RenderPass 结合本状态生成 GraphicsPipelineDesc。
		MaterialRenderStateDesc renderState = {};
	};
}

