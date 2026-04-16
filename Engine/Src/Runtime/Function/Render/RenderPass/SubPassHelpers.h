#pragma once
#include "Function/Render/DX12RHI/DXPipeline/GraphicsPipelineDescBuilder.h"
#include "Function/Render/ResourceManager.h"

namespace photon
{
	inline bool IsSubmeshEnabled(const RenderItem& item, uint32_t submeshIndex)
	{
		if (item.submeshMask == 0)
			return true;
		return ((item.submeshMask >> submeshIndex) & 1ull) != 0;
	}

	inline MaterialHandle ResolveRenderItemMaterial(
		const ResourceManager& rm,
		const RenderItem& item,
		const MeshRenderResource& meshRR,
		uint32_t submeshIndex)
	{
		MeshRendererComponent fakeRenderer = {};
		fakeRenderer.mesh = item.mesh;
		fakeRenderer.overrideMaterials = item.overrideMaterials;
		return ResolveMaterialForSubmesh(rm, fakeRenderer, meshRR, submeshIndex);
	}

	inline ShaderHandle ResolveShaderForMaterial(
		const ResourceManager& rm,
		MaterialHandle material)
	{
		const MaterialRenderResource* matRR = rm.GetMaterialRenderResource(material);
		return matRR ? matRR->shaderProgram : ShaderHandle{};
	}

	inline PipelineStateHandle ResolvePipelineForDrawItem(
		const PassPrepareContext& ctx,
		MaterialHandle material,
		uint64_t vertexLayoutHash,
		const PassPipelineStateOverrides& overrides)
	{
		return GetOrCreateGraphicsPipelineForMaterial(
			*ctx.services->pipelineCache,
			*ctx.services->resourceManager,
			material,
			vertexLayoutHash,
			overrides);
	}
}