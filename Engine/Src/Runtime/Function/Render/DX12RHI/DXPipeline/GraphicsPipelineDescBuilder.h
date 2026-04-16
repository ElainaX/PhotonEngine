#pragma once

#include "GraphicsPipelineState.h"
#include "PipelineStateCache.h"
#include "Function/Render/DX12RHI/RenderResource/ResourceResolver.h"
#include "Function/Render/RenderObject/MaterialRenderState.h"

namespace photon
{
	struct PassPipelineStateOverrides
	{
		RenderTargetLayoutKey rtLayout = {};
		PrimitiveTopologyType topologyType = PrimitiveTopologyType::Triangle;
		SampleDescKey sampleDesc = {};
		uint32_t sampleMask = 0xffffffffu;

		bool hasRasterOverride = false;
		RasterStateDesc rasterOverride = {};

		bool hasDepthStencilOverride = false;
		DepthStencilStateDesc depthStencilOverride = {};

		bool hasBlendOverride = false;
		BlendStateDesc blendOverride = {};

		void Canonicalize();
	};

	namespace PassPipelinePresets
	{
		PassPipelineStateOverrides ForwardColor(TextureFormat colorFormat, TextureFormat depthFormat);
		PassPipelineStateOverrides Skybox(TextureFormat colorFormat, TextureFormat depthFormat);
		PassPipelineStateOverrides ShadowDepth(TextureFormat depthFormat,
			int32_t depthBias, float slopeScaledDepthBias, float depthBiasClamp = 0.0f);
		PassPipelineStateOverrides UI(TextureFormat colorFormat);
	}

	RasterStateDesc BuildRasterStateFromMaterialState(const MaterialRenderStateDesc& materialState);
	DepthStencilStateDesc BuildDepthStencilStateFromMaterialState(const MaterialRenderStateDesc& materialState);
	BlendStateDesc BuildBlendStateFromMaterialState(const MaterialRenderStateDesc& materialState);

	bool BuildGraphicsPipelineDescFromShader(
		const IResourceView& resources,
		ShaderHandle shaderHandle,
		const MaterialRenderStateDesc& materialState,
		uint64_t vertexLayoutHash,
		const PassPipelineStateOverrides& passOverrides,
		GraphicsPipelineDesc& outDesc);

	bool BuildGraphicsPipelineDescForMaterial(
		const IResourceView& resources,
		MaterialHandle materialHandle,
		uint64_t vertexLayoutHash,
		const PassPipelineStateOverrides& passOverrides,
		GraphicsPipelineDesc& outDesc);

	PipelineStateHandle GetOrCreateGraphicsPipelineForMaterial(
		PipelineStateCache& cache,
		const IResourceView& resources,
		MaterialHandle materialHandle,
		uint64_t vertexLayoutHash,
		const PassPipelineStateOverrides& passOverrides);
}
